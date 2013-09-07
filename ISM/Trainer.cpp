#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <iostream>
#include <set>
#include <math.h>

#include "Trainer.hpp"
#include "RecordedPattern.hpp"
#include "MathHelper.hpp"
#include "VoteSpecifier.hpp"
#include "Pose.hpp"
#include "Tracks.hpp"
#include "StaticRelationHeuristic.hpp"
#include "DirectionRelationHeuristic.hpp"
#include "DirectionOrientationRelationHeuristic.hpp"
#include "DataCollector.hpp"

namespace ISM {
    Trainer::Trainer(std::string dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
        this->skips = 0;
        this->useClustering = true;
    }

    void Trainer::setSkipsPerCycle(int skips) {
        this->skips = skips;
    }

    void Trainer::setUseClustering(bool useClustering) {
        this->useClustering = useClustering;
    }

    void Trainer::trainPattern() {
        std::vector<std::string> patternNames = this->tableHelper->getRecordedPatternNames();
        std::cerr<<"found "<<patternNames.size()<<" patterns"<<std::endl;
        for (auto& name : patternNames) {
            this->trainPattern(name);
        }
    }

    void Trainer::trainPattern(const std::string& patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
        if (!r) {
            std::cerr<<"no pattern records found for pattern "<<patternName<<std::endl;
        } else {
            std::cerr<<"training "<<patternName<<std::endl;
            this->recordedPattern = r;
            this->learn();
        }
    }

    void Trainer::learn() {
        std::vector<ObjectSetPtr> sets = this->recordedPattern->objectSets;
        for (size_t i = 0; i < sets.size(); i++) {
            if (sets[i]->objects.size() == 0) {
                sets.erase(sets.begin() + i);
                i--;
            }
        }

        int clusterId = 0;
        TracksPtr tracks(new Tracks(sets));

        while (this->useClustering) {
            auto heuristic = this->findHeuristicMatch(tracks);
            if (!heuristic || (int) tracks->tracks.size() <= 2) {
                break;
            }

            auto cluster = heuristic->cluster;
            std::stringstream subPatternNameStream;
            subPatternNameStream<<this->recordedPattern->name<<"_sub"<<clusterId;
            std::string subPatternName = subPatternNameStream.str();

            auto refTrack = this->doTraining(cluster->toObjectSetVector(), subPatternName);
            clusterId++;
            if (DataCollector::shouldCollect()) {
                DataCollector::getData()->tracksWithRef.push_back(TracksWithRef(cluster, refTrack));
            }
            tracks->replace(cluster->tracks, refTrack);
        }

        //train remaining sets
        auto refTrack = this->doTraining(tracks->toObjectSetVector(), this->recordedPattern->name);
        if (DataCollector::shouldCollect()) {
            DataCollector::getData()->tracksWithRef.push_back(TracksWithRef(tracks, refTrack));
        }
    }

    HeuristicPtr Trainer::findHeuristicMatch(const TracksPtr& tracks) {
        HeuristicPtr bestHeuristic;

        std::vector<HeuristicPtr> heuristics;
        //heuristics.push_back(HeuristicPtr(new StaticRelationHeuristic(tracks)));
        heuristics.push_back(HeuristicPtr(new DirectionRelationHeuristic(tracks)));
        //heuristics.push_back(HeuristicPtr(new DirectionOrientationRelationHeuristic(tracks)));

        for (auto& heuristic : heuristics) {
            if (!heuristic->cluster) {
                continue;
            }
            std::cerr<<"heuristic results of "<<heuristic->name<<std::endl;
            std::cerr<<heuristic->cluster->tracks.size()<<" tracks, confidence: "<<heuristic->confidence<<std::endl;
            if (heuristic->confidence > 0.7 && (!bestHeuristic || heuristic->confidence > bestHeuristic->confidence)) {
                bestHeuristic = heuristic;
            }
        }

        return bestHeuristic ? bestHeuristic : HeuristicPtr();
    }

    TrackPtr Trainer::doTraining(std::vector<ObjectSetPtr> sets, std::string patternName) {
        int toSkip = 0;
        int setCount = 0;
        double objectsWeightSum = 0;
        std::string refType = "";
        std::string refId = "";

        TrackPtr refTrack(new Track(patternName));

        TracksPtr tracks(new Tracks(sets));

        {
            double bestViewRatio = 0;
            double bestMovement = 0;
            for (auto& track : tracks->tracks) {
                int views = 0;
                std::string refT;
                std::string refI;
                ObjectPtr lastObj;
                double movement = 0;
                for (auto& obj : track->objects) {
                    if (obj) {
                        refT = obj->type;
                        refI = obj->observedId;
                        views++;
                        if (lastObj) {
                            movement += MathHelper::getDistanceBetweenPoints(obj->pose->point, lastObj->pose->point);
                        }
                        lastObj = obj;
                    }
                }

                double ratio = (double)views / (double)track->objects.size();
                if (ratio > bestViewRatio || (ratio == bestViewRatio && movement < bestMovement)) {
                    refType = refT;
                    refId = refI;
                    bestViewRatio = ratio;
                    bestMovement = movement;
                }
            }
        }

        std::cerr<<"choose ref "<<refType<<" : "<<refId<<std::endl;
        std::cerr<<"training "<<patternName<<" "<<std::endl;

        for (ObjectSetPtr& os : sets) {
            double setWeightSum = 0;
            if (toSkip == 0) {
                toSkip = this->skips;
                std::cerr<<".";
                std::cerr.flush();
            } else {
                std::cerr<<"_";
                std::cerr.flush();
                toSkip--;
                continue;
            }

            PosePtr referencePose;
            setCount++;
            std::vector<ObjectPtr> objects = os->objects;

            for (auto& o : objects) {
                if (o->type == refType && o->observedId == refId) {
                    referencePose.reset(new Pose(*(o->pose)));
                    break;
                }
            }
            if (!referencePose && objects.size() > 0) {
                referencePose.reset(new Pose(*(objects[0]->pose)));
            } else if (!referencePose) {
                refTrack->objects.push_back(ObjectPtr());
                continue;
            }

            for (ObjectPtr& o : objects) {
                VoteSpecifierPtr vote = MathHelper::getVoteSpecifierToPose(o->pose, referencePose);
                vote->patternName = patternName;
                vote->observedId = o->observedId;
                vote->objectType = o->type;
                this->tableHelper->insertModelVoteSpecifier(vote);
                setWeightSum += o->weight;
            }

            auto refObj = ObjectPtr(
                new Object(
                    patternName,
                    referencePose
                )
            );

            refObj->weight = setWeightSum;

            refTrack->objects.push_back(refObj);

            objectsWeightSum += setWeightSum;

        }

        this->tableHelper->upsertModelPattern(
            patternName,
            floor(((float)objectsWeightSum / (float)setCount) + 0.5)
        );

        refTrack->calculateWeight();

        std::cerr<<"done ("<<refTrack->weight<<")"<<std::endl;

        return refTrack;
    }
}

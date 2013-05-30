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
#include "JsonStream.hpp"
#include "Tracks.hpp"
#include "StaticRelationHeuristic.hpp"
#include "DirectionRelationHeuristic.hpp"

namespace ISM {
    Trainer::Trainer(std::string dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
        this->skips = 0;
    }

    void Trainer::setSkipsPerCycle(int skips) {
        this->skips = skips;
    }

    void Trainer::trainPattern() {
        std::vector<std::string> patternNames = this->tableHelper->getRecordedPatternNames();
        std::cout<<"found "<<patternNames.size()<<" patterns"<<std::endl;
        for (auto& name : patternNames) {
            this->trainPattern(name);
        }
    }

    void Trainer::trainPattern(const std::string& patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
        if (!r) {
            std::cout<<"no pattern records found for pattern "<<patternName<<std::endl;
        } else {
            std::cout<<"training "<<patternName<<std::endl;
            this->recordedPattern = r;
            this->learn(false);
        }
    }

    void Trainer::learn(bool generateJson) {
        typedef MathHelper MH;

        std::vector<ObjectSetPtr> sets = this->recordedPattern->objectSets;
        for (size_t i = 0; i < sets.size(); i++) {
            if (sets[i]->objects.size() == 0) {
                sets.erase(sets.begin() + i);
                i--;
            }
        }

        int clusterId = 0;
        TracksPtr tracks(new Tracks(sets));

        while (true) {
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
            tracks->replace(cluster->tracks, refTrack);
        }

        //train remaining sets
        this->doTraining(tracks->toObjectSetVector(), this->recordedPattern->name);
    }

    HeuristicPtr Trainer::findHeuristicMatch(const TracksPtr& tracks) {
        HeuristicPtr bestHeuristic;

        std::vector<HeuristicPtr> heuristics;
        //heuristics.push_back(HeuristicPtr(new StaticRelationHeuristic(tracks)));
        heuristics.push_back(HeuristicPtr(new DirectionRelationHeuristic(tracks)));

        for (auto& heuristic : heuristics) {
            if (!heuristic->cluster) {
                continue;
            }
            std::cout<<"heuristic results of "<<heuristic->name<<std::endl;
            std::cout<<heuristic->cluster->tracks.size()<<" tracks, confidence: "<<heuristic->confidence<<std::endl;
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
        bool first = true;
        std::string refType = "";
        std::string refId = "";

        TrackPtr refTrack(new Track(patternName));

        TracksPtr tracks(new Tracks(sets));

        {
            double bestViewRatio = 0;
            for (auto& track : tracks->tracks) {
                int views = 0;
                std::string refT;
                std::string refI;
                for (auto& obj : track->objects) {
                    if (obj) {
                        refT = obj->type;
                        refI = obj->observedId;
                        views++;
                    }
                }

                double ratio = (double)views / (double)track->objects.size();
                if (ratio > bestViewRatio) {
                    refType = refT;
                    refId = refI;
                    bestViewRatio = ratio;
                }
            }
        }

        std::cout<<"choose ref "<<refType<<" : "<<refId<<std::endl;
        std::cout<<"training "<<patternName<<" "<<std::endl;

        for (ObjectSetPtr& os : sets) {
            double setWeightSum = 0;
            if (toSkip == 0) {
                toSkip = this->skips;
                std::cout<<".";
                std::cout.flush();
            } else {
                std::cout<<"_";
                std::cout.flush();
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
                //auto rpoint = MH::applyQuatAndRadiusToPose(o->pose, vote->objectToRefQuat, vote->radius);
                //auto rpose = MH::getReferencePose(o->pose, rpoint, vote->objectToRefPoseQuat);
                //auto bpoint = MH::applyQuatAndRadiusToPose(rpose, vote->refToObjectQuat, vote->radius);
                //std::cout<<"projected pose:"<<MH::vectorToPoint(MH::getPoseVectorFromQuat(rpose->quat))<<std::endl;
                //std::cout<<"projected pose errors:"<<
                    //(MH::getPoseVectorFromQuat(rpose->quat) - MH::getPoseVectorFromQuat(referencePose->quat)).norm()
                    //<<","<<MH::getDistanceBetweenPoints(referencePose->point, rpose->point)<<std::endl;
                //std::cout<<"error:"<<MH::getDistanceBetweenPoints(o->pose->point, bpoint)<<std::endl;
            }

            for (auto& obj : objects) {
                setWeightSum += obj->weight;
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

        std::cout<<"done ("<<refTrack->weight<<")"<<std::endl;

        return refTrack;
    }

    std::string Trainer::getJsonRepresentation(const std::string& patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
        if (r) {
            this->json.str("");
            this->json.clear();
            this->recordedPattern = r;
            this->learn(true);
            return this->json.str();
        } else {
            return "";
        }
    }
}

#include "Recognizer.hpp"
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <sstream>
#include <boost/foreach.hpp>

#include "MathHelper.hpp"
#include "VotedPose.hpp"
#include "VotingSpace.hpp"

namespace ISM {
    Recognizer::Recognizer(const std::string& dbfilename, double sensitivity) :
            sensitivity(sensitivity), again(false) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
    }

    Recognizer::Recognizer(double sensitivity, const std::string& dbfilename) :
            sensitivity(sensitivity), again(false) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
    }

    const std::vector<RecognitionResultPtr> Recognizer::recognizePattern(const ObjectSetPtr& objectSet) {
        this->objectTypes.clear();
        this->inputSet = objectSet;

        this->objectTypes = this->tableHelper->getObjectTypes();

        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(this->objectTypes);
        this->getPatternDefinitions();

//        std::cout<<"######"<<std::endl;
        int loops = 0, maxLoops = 2;
        do {
            this->results.clear();
            this->again = false;
//            std::cout<<"-----"<<std::endl;
            this->calculateVotes();
            loops++;

            //std::cout<<this->inputSet<<std::endl;
            //for (auto& res : this->results) {
            //    std::cout<<res<<std::endl;
            //}
        } while (this->again && loops <= maxLoops);
//        std::cout << "InputSet: " << this->inputSet << std::endl;
//        std::cout << "Results: " << std::endl;
//        for (auto& r : this->results) {
//            std::cout << r << std::endl;
//        }
//        return this->results;
        return this->filterResults(this->results);
    }

    void Recognizer::calculateVotes() {
        //vote for each pattern separately
        std::map<PatternPtr, std::vector<VotedPosePtr> > votesMap;

        std::map<ObjectPtr, std::vector<PointPtr>> votedPoints;
        for (ObjectPtr& object : this->inputSet->objects) {
            std::set<std::string> types;
            if (object->type == "") {
                types = this->objectTypes;
            } else {
                types.insert(object->type);
            }

            for (auto& objectType : types) {
                std::vector<VoteSpecifierPtr> votes = this->objectDefinitions[objectType];
                for (VoteSpecifierPtr& vote : votes) {
                    if (object->observedId == "" || object->observedId == vote->observedId) {
                        PatternPtr pattern = this->patternDefinitions[vote->patternName];
                        PosePtr pose = this->calculatePoseFromVote(object->pose, vote);

                        if (votesMap.find(pattern) == votesMap.end()) {
                            votesMap[pattern] = std::vector<VotedPosePtr>();
                        }

                        VotedPosePtr v(
                                new VotedPose(pose, vote, object,
                                        object->weight / pattern->expectedMaxWeight));
                        votesMap[pattern].push_back(v);

                        if (votedPoints.find(object) == votedPoints.end()) {
                            votedPoints[object] = std::vector<PointPtr>();
                        }
                        votedPoints[object].push_back(pose->point);
                    }
                }
            }
        }

        VotingSpace vs(this->sensitivity);

        for (auto& votePair : votesMap) {
            auto vsresults = vs.vote(votePair.second);
            int added = 0;
            for (auto& vsres : vsresults) {
                RecognitionResultPtr res(
                        new RecognitionResult(votePair.first->name, vsres->referencePose, vsres->matchingObjects,
                                vsres->confidence, vsres->idealPoints, votedPoints));
                // if we have an object on record with a name that matches a pattern name
                // treat the referencePose as a new object and repeat the recognition to capture subscenes
                if (this->objectDefinitions.find(res->patternName) != this->objectDefinitions.end()) {
                    ObjectPtr refObj(new Object(res->patternName, res->referencePose));
                    refObj->confidence = vsres->confidence;
                    double weightSum = 0;
                    for (auto& obj : vsres->matchingObjects->objects) {
                        weightSum += obj->weight;
                    }
                    refObj->weight = weightSum;
                    if (!this->objectAlreadyInSet(refObj)) {
                        added++;
                        this->inputSet->insert(refObj);

                        this->again = true;
                    }
                }
                this->results.push_back(res);
            }
            std::cout << "voting round for " << votePair.first->name << " added " << added <<std::endl;
        }
    }

    bool Recognizer::objectAlreadyInSet(const ObjectPtr& o) {
        for (auto& setObj : this->inputSet->objects) {
            if (
                    setObj->type == o->type &&
                    setObj->observedId == o->observedId &&
                    setObj->weight == o->weight &&
                    setObj->pose->point->x == o->pose->point->x &&
                    setObj->pose->point->y == o->pose->point->y &&
                    setObj->pose->point->z == o->pose->point->z &&
                    setObj->pose->quat->x == o->pose->quat->x &&
                    setObj->pose->quat->y == o->pose->quat->y &&
                    setObj->pose->quat->z == o->pose->quat->z &&
                    setObj->pose->quat->w == o->pose->quat->w
            ) {
                return true;
            }
        }
        return false;
    }

    PosePtr Recognizer::calculatePoseFromVote(const PosePtr& pose, const VoteSpecifierPtr& vote) const {
        PointPtr referencePoint = MathHelper::applyQuatAndRadiusToPose(pose, vote->objectToRefQuat, vote->radius);
        return MathHelper::getReferencePose(pose, referencePoint, vote->objectToRefPoseQuat);
    }

    void Recognizer::getPatternDefinitions() {
        typedef std::pair<std::string, std::vector<VoteSpecifierPtr> > mapItemType;
        std::set<std::string> patternNames;
        BOOST_FOREACH(mapItemType item, this->objectDefinitions){
        BOOST_FOREACH(VoteSpecifierPtr vote, item.second) {
            patternNames.insert(vote->patternName);
        }
    }

        this->patternDefinitions = this->tableHelper->getPatternDefinitionsByName(patternNames);
    }

    std::vector<RecognitionResultPtr> Recognizer::filterResults(const std::vector<RecognitionResultPtr>& results) {
        std::map<std::string, std::vector<RecognitionResultPtr> > resultsForPattern;
        for (auto& res : results) {
            auto it = resultsForPattern.find(res->patternName);
            if (it == resultsForPattern.end()) {
                std::vector<RecognitionResultPtr> p;
                p.push_back(res);
                resultsForPattern.insert(std::make_pair(res->patternName, p));
            } else {
                (*it).second.push_back(res);
            }
        }

        std::vector<RecognitionResultPtr> ret;

        for (auto& patternPair : resultsForPattern) {
            auto rlist = patternPair.second;
            std::sort(rlist.begin(), rlist.end(), [](const RecognitionResultPtr& o1, const RecognitionResultPtr& o2) {
                return o1->confidence > o2->confidence;
            });
            ret.push_back(rlist[0]);
        }
        return ret;
    }
}

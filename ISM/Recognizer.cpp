#include "Recognizer.hpp"
#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>

#include "MathHelper.hpp"
#include "VotedPose.hpp"
#include "VotingSpace.hpp"

namespace ISM {
    Recognizer::Recognizer(const std::string& dbfilename, double sensitivity) :
            sensitivity(sensitivity), again(false), loop(0) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
    }

    Recognizer::Recognizer(double sensitivity, const std::string& dbfilename) :
            sensitivity(sensitivity), again(false), loop(0) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
    }

    const std::vector<RecognitionResultPtr> Recognizer::recognizePattern(const ObjectSetPtr& objectSet) {
        this->objectTypes.clear();
        this->inputSet = objectSet;
        this->loop = 0;

        this->objectTypes = this->tableHelper->getObjectTypes();

        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(this->objectTypes);
        this->getPatternDefinitions();

//        std::cout<<"######"<<std::endl;
        do {
            this->results.clear();
            this->again = false;
            this->calculateVotes();
            //std::cout<<"-----"<<std::endl;

            //std::cout<<this->inputSet<<std::endl;
            //for (auto& res : this->results) {
            //    std::cout<<res<<std::endl;
            //}
        } while (this->again);
//        std::cout << "InputSet: " << this->inputSet << std::endl;
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
        std::set<std::string> inputSetTypes;
        for (auto& input : this->inputSet->objects) {
            inputSetTypes.insert(input->type);
        }

        for (auto& votePair : votesMap) {
            auto vsresults = vs.vote(votePair.second);
            for (auto& vsres : vsresults) {
                RecognitionResultPtr res(
                        new RecognitionResult(votePair.first->name, vsres->referencePose, vsres->matchingObjects,
                                vsres->confidence, vsres->idealPoints, votedPoints));
                // if we have an object on record with a name that matches a pattern name
                // treat the referencePose as a new object and repeat the recognition to capture subscenes
                if (this->objectDefinitions.find(res->patternName) != this->objectDefinitions.end() &&
                        this->loop < this->maxLoops) {
                    ObjectPtr refObj(new Object(res->patternName, res->referencePose));
                    refObj->confidence = vsres->confidence;
                    double weightSum = 0;
                    for (auto& obj : vsres->matchingObjects->objects) {
                        weightSum += obj->weight;
                    }
                    refObj->weight = weightSum;
                    this->inputSet->insert(refObj);

                    this->loop++;
                    this->again = true;
                }
                this->results.push_back(res);
            }
        }
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

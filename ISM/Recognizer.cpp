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
        this->objectTypes = this->tableHelper->getObjectTypes();
        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(this->objectTypes);
        this->getPatternDefinitions();
    }

    Recognizer::Recognizer(double sensitivity, const std::string& dbfilename) :
            sensitivity(sensitivity), again(false) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
        this->objectTypes = this->tableHelper->getObjectTypes();
        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(this->objectTypes);
        this->getPatternDefinitions();
    }

    const std::vector<RecognitionResultPtr> Recognizer::recognizePattern(const ObjectSetPtr& objectSet,
            const double filterThreshold) {
        this->inputSet = objectSet;

        int loops = 0, maxLoops = 10;
        do {
            this->results.clear();
            this->again = false;
            this->calculateVotes();
            loops++;

        } while (this->again && loops <= maxLoops);
        return this->filterResults(this->results, filterThreshold);
    }

    void Recognizer::calculateVotes() {
        std::map<PatternPtr, std::vector<VotedPosePtr> > votesMap;

        VotedPointsTypePtr votedPoints(new VotedPointsType());
        int voteCount = 0;
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

                        VotedPosePtr v(new VotedPose(pose, vote, object, object->weight / pattern->expectedMaxWeight));
                        votesMap[pattern].push_back(v);

                        if (votedPoints->find(object) == votedPoints->end()) {
                            (*votedPoints)[object] = std::vector<std::pair<PointPtr, double> >();
                        }
                        (*votedPoints)[object].push_back(std::make_pair(pose->point, v->confidence));
                        voteCount++;
                    }
                }
            }
        }

        VotingSpace vs(this->sensitivity);

        for (auto& votePair : votesMap) {
            auto vsresults = vs.vote(votePair.second);
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
                    auto inSetIdx = this->objectAlreadyInSet(refObj);
                    if (inSetIdx >= 0) {
                        auto setObj = this->inputSet->objects[inSetIdx];
                        if (setObj->confidence < refObj->confidence || setObj->weight < refObj->weight) {
                            this->inputSet->objects.erase(this->inputSet->objects.begin() + inSetIdx);
                            inSetIdx = -1;
                        }
                    }
                    if (inSetIdx < 0) {
                        this->inputSet->insert(refObj);

                        this->again = true;
                    }
                }
                this->results.push_back(res);
            }
        }
    }

    int Recognizer::objectAlreadyInSet(const ObjectPtr& o) {
        for (size_t i = 0; i < this->inputSet->objects.size(); i++) {
            auto setObj = this->inputSet->objects[i];
            if (setObj->type == o->type && setObj->observedId == o->observedId
                    && Recognizer::poseEqual(setObj->pose, o->pose)) {
                return i;
            }
        }
        return -1;
    }

    bool Recognizer::poseEqual(const PosePtr& p1, const PosePtr& p2) {
        return fabs(p1->point->x - p2->point->x) < 0.00001 && fabs(p1->point->y - p2->point->y) < 0.00001
                && fabs(p1->point->z - p2->point->z) < 0.00001 && fabs(p1->quat->x - p2->quat->x) < 0.00001
                && fabs(p1->quat->y - p2->quat->y) < 0.00001 && fabs(p1->quat->z - p2->quat->z) < 0.00001
                && fabs(p1->quat->w - p2->quat->w) < 0.00001;
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

    std::vector<RecognitionResultPtr> Recognizer::filterResults(const std::vector<RecognitionResultPtr>& results,
            const double filterThreshold) {
        std::map<std::string, std::vector<RecognitionResultPtr> > patternNameToResults;
        std::vector<RecognitionResultPtr> topLevelResults;
        for (auto& res : results) {
            if (res->patternName.find("_sub") == std::string::npos) {
                topLevelResults.push_back(res);
            }
            auto it = patternNameToResults.find(res->patternName);
            if (it == patternNameToResults.end()) {
                it =
                        patternNameToResults.insert(
                                std::make_pair(res->patternName, std::vector<RecognitionResultPtr>())).first;
            }
            (*it).second.push_back(res);
        }

        std::map<std::string, std::vector<RecognitionResultPtr> > resultsForPattern;
        for (auto& res : topLevelResults) {
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
            for (size_t i = 0; i < rlist.size(); i++) {
                if (rlist[i]->confidence >= filterThreshold) {
                    auto r = rlist[i];
                    ret.push_back(r);
                    r->subPatterns = Recognizer::getSubPatternsForResult(r, patternNameToResults);
                } else {
                    break;
                }
            }
        }

        std::sort(ret.begin(), ret.end(), [](const RecognitionResultPtr& o1, const RecognitionResultPtr& o2) {
            return o1->confidence > o2->confidence;
        });

        return ret;
    }

    std::vector<RecognitionResultPtr> Recognizer::getSubPatternsForResult(RecognitionResultPtr result,
            std::map<std::string, std::vector<RecognitionResultPtr> > patternNameToResults) {
        std::vector<RecognitionResultPtr> ret;
        for (auto& obj : result->recognizedSet->objects) {
            auto it = patternNameToResults.find(obj->type);
            if (it == patternNameToResults.end()) {
                continue;
            }
            for (auto& originalResult : (*it).second) {
                if (Recognizer::poseEqual(obj->pose, originalResult->referencePose)
                        && fabs(obj->confidence - originalResult->confidence) < 0.01) {
                    ret.push_back(originalResult);
                }
            }
        }

        for (auto& subPattern : ret) {
            subPattern->subPatterns = Recognizer::getSubPatternsForResult(subPattern, patternNameToResults);
        }
        return ret;
    }
}

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
    Recognizer::Recognizer(const std::string& dbfilename, double sensitivity) : sensitivity(sensitivity) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
    }

    Recognizer::Recognizer(double sensitivity, const std::string& dbfilename) : sensitivity(sensitivity) {
        this->tableHelper = TableHelperPtr(new TableHelper(dbfilename));
    }

    const std::vector<RecognitionResultPtr> Recognizer::recognizePattern(const ObjectSetPtr& objectSet) {
        this->objectTypes.clear();
        this->inputSet = objectSet;

        this->objectTypes = this->tableHelper->getObjectTypes();

        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(this->objectTypes);
        this->getPatternDefinitions();

        //std::cout<<"######"<<std::endl;
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
        return this->results;
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
                            new VotedPose(pose, vote, object, (1.0 / pattern->expectedObjectCount) * object->confidence)
                        );
                        votesMap[pattern].push_back(v);

                        if (votedPoints.find(object) == votedPoints.end()) {
                            votedPoints[object] = std::vector<PointPtr>();
                        }
                        votedPoints[object].push_back(pose->point);
                    }
                }
            }
        }

        std::set<std::string> typesInInputSet;
        for (auto& obj : this->inputSet->objects) {
            typesInInputSet.insert(obj->type);
        }

        for (auto& votePair : votesMap) {
            VotingSpace vs(votePair.second, this->sensitivity);
            if (vs.confidence != -1.0) {
                RecognitionResultPtr res(
                    new RecognitionResult(
                        votePair.first->name,
                        vs.referencePose,
                        vs.matchingObjects,
                        vs.confidence,
                        vs.idealPoints,
                        votedPoints
                    )
                );
                // if we have an object on record with a name that matches a pattern name
                // treat the referencePose as a new object and repeat the recognition to capture subscenes
                if (
                    this->objectDefinitions.find(res->patternName) != this->objectDefinitions.end() &&
                    typesInInputSet.find(res->patternName) == typesInInputSet.end()
                ) {
                    ObjectPtr refObj(new Object(res->patternName, res->referencePose));
                    refObj->confidence = vs.confidence;
                    this->inputSet->insert(refObj);
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
        BOOST_FOREACH(mapItemType item, this->objectDefinitions) {
            BOOST_FOREACH(VoteSpecifierPtr vote, item.second) {
                patternNames.insert(vote->patternName);
            }
        }

        this->patternDefinitions = this->tableHelper->getPatternDefinitionsByName(patternNames);
    }
}

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
    Recognizer::Recognizer(const std::string& dbfilename, int sensitivity) : sensitivity(sensitivity) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    Recognizer::Recognizer(int sensitivity, const std::string& dbfilename) : sensitivity(sensitivity) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    const std::vector<RecognitionResultPtr> Recognizer::recognizePattern(const ObjectSetPtr& objectSet) {
        this->inputSet = objectSet;
        std::set<std::string> objectTypes;
        BOOST_FOREACH(ObjectPtr o, objectSet->objects) {
            objectTypes.insert(o->type);
        }

        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(objectTypes);
        this->getPatternDefinitions();

        this->calculateVotes();
        return this->results;
    }

    void Recognizer::calculateVotes() {
        //vote for each pattern separately
        std::map<PatternPtr, std::vector<VotedPosePtr> > votesMap;

        for (ObjectPtr& object : this->inputSet->objects) {
            std::vector<VoteSpecifierPtr> votes = this->objectDefinitions[object->type];
            for (VoteSpecifierPtr& vote : votes) {
                PatternPtr pattern = this->patternDefinitions[vote->patternName];
                PosePtr pose = this->calculatePoseFromVote(object->pose, vote);

                if (votesMap.find(pattern) == votesMap.end()) {
                    votesMap[pattern] = std::vector<VotedPosePtr>();
                }

                VotedPosePtr v(new VotedPose(pose, vote, object, 1.0 / pattern->expectedObjectCount));
                votesMap[pattern].push_back(v);
            }
        }

        for (auto& votePair : votesMap) {
            VotingSpace vs(votePair.second, this->sensitivity);
            this->results.push_back(
                RecognitionResultPtr(
                    new RecognitionResult(
                        votePair.first->name,
                        vs.referencePose,
                        vs.matchingObjects,
                        vs.confidence
                    )
                )
            );
        }
    }

    PosePtr Recognizer::calculatePoseFromVote(PosePtr pose, VoteSpecifierPtr vote) {
        PointPtr referencePoint = MathHelper::applyQuatAndRadiusToPose(pose, vote->objectToRefQuat, vote->radius);
        return MathHelper::getReferencePose(pose, referencePoint, vote->refToObjectQuat);
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

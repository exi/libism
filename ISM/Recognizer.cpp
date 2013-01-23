#include "Recognizer.hpp"

#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>

#include "MathHelper.hpp"

namespace ISM {
    Recognizer::Recognizer(std::string dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    std::vector<RecognitionResultPtr> Recognizer::recognizePattern(ObjectSetPtr objectSet) {
        this->inputSet = objectSet;
        std::set<std::string> objectTypes;
        BOOST_FOREACH(ObjectPtr o, objectSet->objects) {
            objectTypes.insert(o->type);
        }

        this->objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(objectTypes);
        this->getPatternDefinitions();

        this->calculateVotes();
        this->calculateReferencePose();
        std::vector<RecognitionResultPtr> ret;
        return ret;
    }

    void Recognizer::calculateVotes() {
        //vote for each pattern separately
        std::map<PatternPtr, std::vector<VotedPosePtr> > votesMap;

        BOOST_FOREACH(ObjectPtr object, this->inputSet->objects) {
            std::vector<VoteSpecifierPtr> votes = this->objectDefinitions[object->type];
            BOOST_FOREACH(VoteSpecifierPtr vote, votes) {
                PatternPtr pattern = this->patternDefinitions[vote->patternName];
                PosePtr pose = this->calculatePoseFromVote(object->pose, vote);

                if (votesMap.find(pattern) == votesMap.end()) {
                    votesMap[pattern] = std::vector<VotedPosePtr>();
                }

                VotedPosePtr v(new VotedPose(pose, vote, object));
                votesMap[pattern].push_back(v);
            }
        }
        typedef std::pair<PatternPtr, std::vector<VotedPosePtr> > votesType;
        BOOST_FOREACH(votesType v, votesMap) {
            std::cout<<"voted for: "<<v.first<<std::endl;
            BOOST_FOREACH(VotedPosePtr p, v.second) {
                std::cout<<
                    p->source->type<<";"<<
                    p->pose->point<<";"<<
                    MathHelper::vectorToPoint(MathHelper::getPoseVectorFromQuat(p->pose->quat))<<
                    std::endl;
            }
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

    void Recognizer::calculateReferencePose() {
        BOOST_FOREACH(ObjectPtr o, this->inputSet->objects) {
            std::vector<VoteSpecifierPtr> votes = this->objectDefinitions[o->type];
        }
    }
}

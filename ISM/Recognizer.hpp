#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "ObjectSet.hpp"
#include "TableHelper.hpp"

namespace ISM {
    struct RecognitionResult {
        std::string patternName;
        ObjectSetPtr recognisedSet;
        double certainty;
    };

    typedef boost::shared_ptr<RecognitionResult> RecognitionResultPtr;

    struct VotedPose {
        PosePtr pose;
        VoteSpecifierPtr vote;
        ObjectPtr source;
        VotedPose(PosePtr pose, VoteSpecifierPtr vote, ObjectPtr source):
            pose(pose), vote(vote), source(source) {};
    };

    typedef boost::shared_ptr<VotedPose> VotedPosePtr;

    class Recognizer {
        TableHelperPtr tableHelper;
        ObjectSetPtr inputSet;

        ObjectTypeToVoteMap objectDefinitions;
        PatternNameToPatternMap patternDefinitions;

        PosePtr referencePose;

        public:
            Recognizer(std::string dbfilename = "record.sqlite");
            std::vector<RecognitionResultPtr> recognizePattern(ObjectSetPtr objectSet);

        private:
            void calculateVotes();
            void calculateReferencePose();
            void getPatternDefinitions();
            PosePtr calculatePoseFromVote(PosePtr pose, VoteSpecifierPtr vote);
    };
}

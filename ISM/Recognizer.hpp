#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "ObjectSet.hpp"
#include "TableHelper.hpp"
#include "RecognitionResult.hpp"

namespace ISM {
    class Recognizer {
        TableHelperPtr tableHelper;
        ObjectSetPtr inputSet;
        int sensitivity;

        ObjectTypeToVoteMap objectDefinitions;
        PatternNameToPatternMap patternDefinitions;

        PosePtr referencePose;

        std::vector<RecognitionResultPtr> results;

        public:
            Recognizer(const std::string& dbfilename = "record.sqlite", int sensitivity = 5);
            Recognizer(int sensitivity = 5, const std::string& dbfilename = "record.sqlite");
            const std::vector<RecognitionResultPtr> recognizePattern(const ObjectSetPtr& objectSet);

        private:
            void calculateVotes();
            void getPatternDefinitions();
            PosePtr calculatePoseFromVote(PosePtr pose, VoteSpecifierPtr vote);
    };
}

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
        double sensitivity;
        bool again = false;
        std::set<std::string> objectTypes;

        ObjectTypeToVoteMap objectDefinitions;
        PatternNameToPatternMap patternDefinitions;

        PosePtr referencePose;

        std::vector<RecognitionResultPtr> results;

        public:
            Recognizer(const std::string& dbfilename = "record.sqlite", double sensitivity = 5);
            Recognizer(double sensitivity = 5, const std::string& dbfilename = "record.sqlite");
            const std::vector<RecognitionResultPtr> recognizePattern(const ObjectSetPtr& objectSet);

        private:
            void calculateVotes();
            void getPatternDefinitions();
            PosePtr calculatePoseFromVote(const PosePtr& pose, const VoteSpecifierPtr& vote) const;
    };
    typedef boost::shared_ptr<Recognizer> RecognizerPtr;
}

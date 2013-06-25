#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "ObjectSet.hpp"
#include "TableHelper.hpp"
#include "RecognitionResult.hpp"
#include "VotedPose.hpp"

namespace ISM {
    class Recognizer {
            TableHelperPtr tableHelper;
            ObjectSetPtr inputSet;
            double sensitivity;
            bool again;
            std::set<std::string> objectTypes;

            ObjectTypeToVoteMap objectDefinitions;
            PatternNameToPatternMap patternDefinitions;

            PosePtr referencePose;

            std::vector<RecognitionResultPtr> results;
            std::map<ObjectPtr, std::vector<VotedPosePtr> > votingCache;

        public:
            Recognizer(const std::string& dbfilename = "record.sqlite", double sensitivity = 5);
            Recognizer(double sensitivity = 5, const std::string& dbfilename = "record.sqlite");
            const std::vector<RecognitionResultPtr> recognizePattern(const ObjectSetPtr& objectSet,
                    const double filterThreshold = 0.0, const int resultsPerPattern = -1);

        private:
            void calculateVotes();
            void getPatternDefinitions();
            int objectAlreadyInSet(const ObjectPtr& o);
            static bool poseEqual(const PosePtr& p1, const PosePtr& p2);
            static std::vector<RecognitionResultPtr> filterResults(const std::vector<RecognitionResultPtr>& results,
                    const double filterThreshold, const int resultsPerPattern);
            PosePtr calculatePoseFromVote(const PosePtr& pose, const VoteSpecifierPtr& vote) const;
            static std::vector<RecognitionResultPtr> getSubPatternsForResult(RecognitionResultPtr result,
                    std::map<std::string, std::vector<RecognitionResultPtr> > patternNameToResults);
    };
    typedef boost::shared_ptr<Recognizer> RecognizerPtr;
}

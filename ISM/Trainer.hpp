#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "ObjectSet.hpp"
#include "Object.hpp"
#include "TableHelper.hpp"
#include "Pose.hpp"
#include "Heuristic.hpp"

namespace ISM {
    class Trainer {
        TableHelperPtr tableHelper;
        RecordedPatternPtr recordedPattern;
        PointPtr absoluteReferencePoint;
        std::stringstream json;
        int skips;
        bool useClustering;
        public:
            Trainer(std::string dbfilename = "record.sqlite");

            void trainPattern();
            void trainPattern(const std::string& patternName);
            void setSkipsPerCycle(const int skips);
            void setUseClustering(const bool useClustering);
            std::string getJsonRepresentation(const std::string& patternName);

        private:
            void learn(bool generateJson);
            HeuristicPtr findHeuristicMatch(const TracksPtr& tracks);
            TrackPtr doTraining(const std::vector<ObjectSetPtr> sets, std::string patternName);
            void jsonPutPoint(const PointPtr& point);
            void jsonPutPose(const PosePtr& pose);
            void jsonPutQuaternion(const QuaternionPtr& quat);
            void jsonPutObjectAndVote(const ObjectPtr& object, const VoteSpecifierPtr& vote);
    };
}

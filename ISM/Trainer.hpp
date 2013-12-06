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
        int skips;
        bool useClustering;
        public:
            Trainer(std::string dbfilename = "record.sqlite");

            void trainPattern();
            void trainPattern(const std::string& patternName);
            void setSkipsPerCycle(const int skips);
            void setUseClustering(const bool useClustering);

        private:
            void learn();
            HeuristicPtr findHeuristicMatch(const TracksPtr& tracks);
            TrackPtr doTraining(const std::vector<ObjectSetPtr> sets, std::string patternName);
    };
}

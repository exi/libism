#pragma once

#include <string>
#include <boost/shared_ptr.hpp>
#include "ObjectSet.hpp"
#include "Object.hpp"
#include "TableHelper.hpp"
#include "Pose.hpp"

namespace ISM {
    class Trainer {
        TableHelper tableHelper;
        RecordedPatternPtr recordedPattern;
        PointPtr absoluteReferencePoint;
        public:
            Trainer(std::string dbfilename = "record.sqlite");

            void trainPattern(std::string patternName);

        private:
            void learn();
    };
}

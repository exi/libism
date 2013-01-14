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
        std::string patternName;
        RecordedPatternPtr recordedPattern;
        Point absoluteReferencePoint;
        public:
            Trainer(std::string dbfilename = "record.sqlite");

            ~Trainer();

            void trainPattern(std::string patternName);

        private:
            void learn();
            void calculateAbsoluteReferencePoint();
    };
}

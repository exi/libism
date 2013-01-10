#pragma once

#include "ObjectSet.hpp"
#include "Object.hpp"
#include <string>
#include <boost/shared_ptr.hpp>
#include "TableHelper.hpp"

namespace ISM {
    class Trainer {
        TableHelper tableHelper;
        public:
            Trainer(std::string dbfilename = "record.sqlite");

            ~Trainer();

            void trainPattern(std::string patternName);

            void learn(boost::shared_ptr<RecordedPattern> pattern, std::string patternName);
    };
}

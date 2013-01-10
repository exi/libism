#pragma once

#include "ObjectSet.hpp"
#include "Object.hpp"
#include <string>
#include <boost/shared_ptr.hpp>
#include "TableHelper.hpp"

namespace ISM {
    class Recorder {
        TableHelper tableHelper;
        public:
            Recorder(std::string dbfilename = "record.sqlite");

            ~Recorder();

            void insert(boost::shared_ptr<ObjectSet> set, std::string patternName);
    };
}

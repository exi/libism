#pragma once

#include "ObjectSet.hpp"
#include "Object.hpp"
#include <string>
#include <boost/shared_ptr.hpp>
#include "TableHelper.hpp"

namespace ISM {
    class Recorder {
        TableHelperPtr tableHelper;
        public:
            Recorder(const std::string& dbfilename = "record.sqlite");

            ~Recorder();

            void insert(const ObjectSetPtr& set, const std::string& patternName);
    };
}

#pragma once

#include <string>
#include <boost/shared_ptr.hpp>

#include "ObjectSet.hpp"
#include "TableHelper.hpp"

namespace ISM {
    struct RecognitionResult {
        std::string patternName;
        ObjectSetPtr recognisedSet;
        double certainty;
    };

    typedef boost::shared_ptr<RecognitionResult> RecognitionResultPtr;

    class Recognizer {
        TableHelperPtr tableHelper;
        public:
            Recognizer(std::string dbfilename = "record.sqlite");
            std::vector<RecognitionResultPtr> recognizePattern(ObjectSetPtr objectSet);
    };
}

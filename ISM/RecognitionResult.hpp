#pragma once

#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "ObjectSet.hpp"
#include "Pose.hpp"

namespace ISM {
    struct RecognitionResult {
        std::string patternName;
        PosePtr referencePose;
        ObjectSetPtr recognizedSet;
        double confidence;
        RecognitionResult(const std::string patternName, const PosePtr referencePose, const ObjectSetPtr recognizedSet, const double confidence) :
            patternName(patternName), referencePose(referencePose), recognizedSet(recognizedSet), confidence(confidence) {};
    };

    typedef boost::shared_ptr<RecognitionResult> RecognitionResultPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResult &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResultPtr &o);
}

#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include "ObjectSet.hpp"
#include "Pose.hpp"

namespace ISM {
    struct RecognitionResult {
        typedef std::map<ObjectPtr, std::vector<PointPtr>> VotedPointsType;
        typedef boost::shared_ptr<VotedPointsType> VotedPointsTypePtr;
        std::string patternName;
        PosePtr referencePose;
        ObjectSetPtr recognizedSet;
        double confidence;
        std::vector<PointPtr> idealPoints;
        VotedPointsTypePtr votedPoints;
        RecognitionResult(
                const std::string patternName,
                const PosePtr referencePose,
                const ObjectSetPtr recognizedSet,
                const double confidence,
                const std::vector<PointPtr> idealPoints,
                const VotedPointsTypePtr votedPoints
                ) :
            patternName(patternName),
            referencePose(referencePose),
            recognizedSet(recognizedSet),
            confidence(confidence),
            idealPoints(idealPoints),
            votedPoints(votedPoints) {};
    };

    typedef boost::shared_ptr<RecognitionResult> RecognitionResultPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResult &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::RecognitionResultPtr &o);
}

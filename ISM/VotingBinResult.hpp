#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include "ObjectSet.hpp"
#include "Pose.hpp"
#include "Point.hpp"

namespace ISM {
    struct VotingBinResult {
        ObjectSetPtr matchingObjects;
        PosePtr referencePose;
        double confidence;
        std::vector<PointPtr> idealPoints;

        VotingBinResult(const VotingBinResult& other): confidence(other.confidence) {
            matchingObjects = ObjectSetPtr(new ObjectSet(*(other.matchingObjects)));
            referencePose = PosePtr(new Pose(*(other.referencePose)));
        }

        VotingBinResult(ObjectSetPtr& os, PosePtr& p, double c): matchingObjects(os), referencePose(p), confidence(c) {};
        VotingBinResult(ObjectSetPtr& os, PosePtr& p, double c, std::vector<PointPtr> i): matchingObjects(os), referencePose(p), confidence(c), idealPoints(i) {};
    };

    typedef boost::shared_ptr<VotingBinResult> VotingBinResultPtr;
} /* VotingBinResult */

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include "Point.hpp"
#include "VotedPose.hpp"
#include "ObjectSet.hpp"

namespace ISM {
    struct VotingSpaceResult {
        PosePtr referencePose;
        ObjectSetPtr matchingObjects;
        double confidence;
        std::vector<PointPtr> idealPoints;

        VotingSpaceResult() {};

        VotingSpaceResult(const VotingSpaceResult& other): confidence(other.confidence) {
            this->matchingObjects = ObjectSetPtr(new ObjectSet(*(other.matchingObjects)));
            this->referencePose = PosePtr(new Pose(*(other.referencePose)));
            this->idealPoints = other.idealPoints;
        }
    };

    typedef boost::shared_ptr<VotingSpaceResult> VotingSpaceResultPtr;
}

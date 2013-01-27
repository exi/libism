#pragma once

#include <boost/shared_ptr.hpp>
#include "Pose.hpp"
#include "VoteSpecifier.hpp"
#include "Object.hpp"

namespace ISM {
    struct VotedPose {
        PosePtr pose;
        VoteSpecifierPtr vote;
        ObjectPtr source;
        double confidence;
        VotedPose(PosePtr pose, VoteSpecifierPtr vote, ObjectPtr source, double confidence):
            pose(pose), vote(vote), source(source), confidence(confidence) {};
    };

    typedef boost::shared_ptr<VotedPose> VotedPosePtr;
}

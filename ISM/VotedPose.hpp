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
        double weight;

        VotedPose(const VotedPose& other): weight(other.weight) {
            pose = PosePtr(new Pose(*(other.pose)));
            vote = VoteSpecifierPtr(new VoteSpecifier(*(other.vote)));
            source = ObjectPtr(new Object(*(other.source)));
        }
        VotedPose(PosePtr pose, VoteSpecifierPtr vote, ObjectPtr source, double weight):
            pose(pose), vote(vote), source(source), weight(weight) {};
    };

    typedef boost::shared_ptr<VotedPose> VotedPosePtr;
}

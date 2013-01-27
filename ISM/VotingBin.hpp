#pragma once

#include <map>
#include "VotedPose.hpp"
#include "ObjectSet.hpp"
#include "Pose.hpp"

namespace ISM {
    class VotingBin {
        public:
            std::map<ObjectPtr, VotedPosePtr> votes;
            double value;
            VotingBin():value(0) {};

            void insert(const VotedPosePtr& vote);
            const PosePtr getReferencePose() const;
            const ObjectSetPtr getObjectSet() const;
    };

    typedef boost::shared_ptr<VotingBin> VotingBinPtr;
}

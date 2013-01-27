#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include "Point.hpp"
#include "VotedPose.hpp"
#include "ObjectSet.hpp"
#include "VotingBin.hpp"

namespace ISM {
    class VotingSpace {
        std::vector<VotedPosePtr> votes;
        double binSize;
        std::map<int , std::map<int, std::map<int, VotingBinPtr> > > voteMap;
        public:
            PosePtr referencePose;
            ObjectSetPtr matchingObjects;
            double confidence;

            VotingSpace(const std::vector<VotedPosePtr>& votes, double binSize);
        private:
            VotingBinPtr getBin(double x, double y, double z);
    };

    typedef boost::shared_ptr<VotingSpace> VotingSpacePtr;
}

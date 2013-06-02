#pragma once

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include "Point.hpp"
#include "VotedPose.hpp"
#include "ObjectSet.hpp"
#include "VotingBin.hpp"

namespace ISM {
    typedef VotingBinResult VotingSpaceResult;
    typedef VotingBinResultPtr VotingSpaceResultPtr;
    class VotingSpace {
        double binSize;
        std::map<int , std::map<int, std::map<int, VotingBinPtr> > > voteMap;
        public:
            VotingSpace(double binSize = 1.0) : binSize(binSize) { };

            std::vector<VotingSpaceResultPtr> vote(std::vector<VotedPosePtr>& votes);
        private:
            VotingBinPtr getBin(double x, double y, double z);
    };

    typedef boost::shared_ptr<VotingSpace> VotingSpacePtr;
}

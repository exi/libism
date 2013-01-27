#include "VotingSpace.hpp"

#include <boost/foreach.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <algorithm>
#include "MathHelper.hpp"

namespace ISM {
    VotingSpace::VotingSpace(const std::vector<VotedPosePtr>& votes, double binSize):
                votes(votes), binSize(binSize), confidence(0.0) {
        for (const VotedPosePtr& vote : votes) {
            PointPtr point = vote->pose->point;
            VotingBinPtr bin = this->getBin(point->x, point->y, point->z);
            bin->insert(vote);
        }

        double maxValue = -1;
        VotingBinPtr maxBin;

        for (auto& xitem : this->voteMap) {
            for (auto& yitem : xitem.second) {
                for (auto& zitem : yitem.second) {
                    VotingBinPtr bin = zitem.second;
                    if (bin->value > maxValue) {
                        maxValue = bin->value;
                        maxBin = bin;
                    }
                }
            }
        }

        if (maxValue != -1) {
            this->referencePose = maxBin->getReferencePose();
            this->confidence = maxValue;
            this->matchingObjects = maxBin->getObjectSet();
        }
    }

    VotingBinPtr VotingSpace::getBin(double x, double y, double z) {
        int binx = (int)(x / this->binSize);
        int biny = (int)(y / this->binSize);
        int binz = (int)(z / this->binSize);

        if (this->voteMap.find(binx) == this->voteMap.end()) {
            this->voteMap.insert(std::make_pair(binx, std::map<int, std::map<int, VotingBinPtr> >()));
        }

        if (this->voteMap[binx].find(biny) == this->voteMap[binx].end()) {
            this->voteMap[binx].insert(std::make_pair(biny, std::map<int, VotingBinPtr>()));
        }

        if (this->voteMap[binx][biny].find(binz) == this->voteMap[binx][biny].end()) {
            this->voteMap[binx][biny].insert(std::make_pair(binz, VotingBinPtr(new VotingBin())));
        }

        return this->voteMap[binx][biny][binz];
    }
}

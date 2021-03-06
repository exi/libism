#include "VotingSpace.hpp"

#include <boost/foreach.hpp>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <map>
#include <algorithm>
#include "MathHelper.hpp"

namespace ISM {
    std::vector<VotingSpaceResultPtr> VotingSpace::vote(std::vector<VotedPosePtr>& votes) {
        this->voteMap.clear();

        std::sort(votes.begin(), votes.end(), [](const VotedPosePtr& v1, const VotedPosePtr& v2) {
            return v1->weight > v2->weight;
        });
        for (const VotedPosePtr& vote : votes) {
            PointPtr point = vote->pose->point;
            VotingBinPtr bin = this->getBin(point->x, point->y, point->z);
            bin->insert(vote);
        }

        std::vector<VotingSpaceResultPtr> results;

        for (auto& xitem : this->voteMap) {
            for (auto& yitem : xitem.second) {
                for (auto& zitem : yitem.second) {
                    VotingBinResultPtr res = zitem.second->getResult(this->binSize);
                    if (res && res->confidence > 0) {
                        results.push_back(res);
                    }
                }
            }
        }

        return results;
    }

    VotingBinPtr VotingSpace::getBin(double x, double y, double z) {
        int binx = (int) (x / this->binSize);
        int biny = (int) (y / this->binSize);
        int binz = (int) (z / this->binSize);

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

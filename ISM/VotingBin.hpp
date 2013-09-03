#pragma once

#include <map>
#include "VotedPose.hpp"
#include "ObjectSet.hpp"
#include "Pose.hpp"
#include "Pattern.hpp"
#include "VotingBinResult.hpp"
#include <stack>
#include <set>
#include <vector>

namespace ISM {
    class VotingBin {
        public:
            typedef std::map<std::string, std::vector<VotedPosePtr> > IdToVoteMap;
            typedef std::map<std::string, IdToVoteMap> TypeToInnerMap;
            TypeToInnerMap votes;
            VotingBin() {};

            void insert(const VotedPosePtr& vote);
            VotingBinResultPtr getResult(double sensitivity);
        private:
            std::set<ObjectPtr> sources;
            std::stack<VotedPosePtr> fittingStack;
            std::vector<PointPtr> idealPoints;
            TypeToInnerMap::iterator currentType;
            IdToVoteMap::iterator currentId;
            std::set<ObjectPtr> takenSources;
            PosePtr fittingPose;
            void searchFit(double sensitivity);
            void addVoteToStack(const VotedPosePtr& v);
    };

    typedef boost::shared_ptr<VotingBin> VotingBinPtr;
}

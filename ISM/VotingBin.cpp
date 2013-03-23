#include "VotingBin.hpp"
#include "MinMaxFinder.hpp"
#include "MathHelper.hpp"
#include <map>

namespace ISM {
    void VotingBin::insert(const VotedPosePtr& vote) {
        auto typeIt = votes.find(vote->source->type);
        if (typeIt == votes.end()) {
            typeIt = votes.insert(std::make_pair(vote->source->type, IdToVoteMap())).first;
        }

        auto idIt = typeIt->second.find(vote->vote->observedId);
        if (idIt == typeIt->second.end()) {
            idIt = typeIt->second.insert(std::make_pair(vote->vote->observedId, std::vector<VotedPosePtr>())).first;
        }

        idIt->second.push_back(vote);
    }

    VotingBinResultPtr VotingBin::getResults(double sensitivity) {
        bool finished = false;
        auto typeIt = votes.begin();
        auto typeItEnd = votes.end();
        for (; typeIt != typeItEnd && !finished; typeIt++) {
            auto idIt = typeIt->second.begin();
            auto idItEnd = typeIt->second.end();
            for (; idIt != idItEnd && !finished; idIt++) {
                auto voteIt = idIt->second.begin();
                auto voteItEnd = idIt->second.end();
                for (; voteIt != voteItEnd && !finished; voteIt++) {
                    fittingPose = (*voteIt)->pose;
                    currentType = typeIt;
                    currentId = idIt;
                    fittingStack = std::stack<VotedPosePtr>();
                    fittingStack.push(*voteIt);
                    takenSources.clear();
                    takenSources.insert((*voteIt)->source);
                    idealPoints.clear();
                    PointPtr projectedPoint = MathHelper::getOriginPoint(
                        fittingPose,
                        (*voteIt)->vote->refToObjectQuat,
                        (*voteIt)->vote->radius
                    );
                    idealPoints.push_back(projectedPoint);
                    finished = searchFit(sensitivity);
                }
            }
        }

        ObjectSetPtr os(new ObjectSet());
        if (finished) {
            double confidence = 0;
            while (!fittingStack.empty()) {
                VotedPosePtr p = fittingStack.top();
                fittingStack.pop();
                confidence += p->confidence;
                ObjectPtr o(new Object(*(p->source)));
                o->observedId = p->vote->observedId;
                os->insert(o);
            }

            return VotingBinResultPtr(new VotingBinResult(os, fittingPose, confidence, idealPoints));
        } else {
            PosePtr p;
            return VotingBinResultPtr(new VotingBinResult(os, p, -1.0));
        }
    }

    bool VotingBin::searchFit(double sensitivity) {
        auto typeIt = votes.begin();
        auto typeItEnd = votes.end();

        for (; typeIt != typeItEnd; typeIt++) {
            auto idIt = typeIt->second.begin();
            auto idItEnd = typeIt->second.end();
            for (; idIt != idItEnd; idIt++) {
                if (typeIt == currentType && idIt == currentId) {
                    continue;
                }

                auto voteIt = idIt->second.begin();
                auto voteItEnd = idIt->second.end();
                VotedPosePtr vote;
                for (; voteIt != voteItEnd; voteIt++) {
                    vote = *voteIt;

                    if (takenSources.find(vote->source) != takenSources.end()) {
                        continue;
                    }

                    PointPtr projectedPoint = MathHelper::getOriginPoint(
                        fittingPose,
                        vote->vote->refToObjectQuat,
                        vote->vote->radius
                    );

                    double distance = MathHelper::getDistanceBetweenPoints(vote->source->pose->point, projectedPoint);
                    if (distance <= sensitivity) {
                        idealPoints.push_back(projectedPoint);
                        //found fit
                        break;
                    }
                }

                if (voteIt == voteItEnd) {
                    return false;
                } else {
                    takenSources.insert(vote->source);
                    fittingStack.push(vote);
                }
            }
        }
        return true;
    }
}

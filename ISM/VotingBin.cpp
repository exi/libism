#include "VotingBin.hpp"
#include "MinMaxFinder.hpp"
#include "MathHelper.hpp"
#include <map>

namespace ISM {
    void VotingBin::insert(const VotedPosePtr& vote) {
        auto typeIt = votes.find(vote->vote->objectType);
        if (typeIt == votes.end()) {
            typeIt = votes.insert(std::make_pair(vote->vote->objectType, IdToVoteMap())).first;
        }

        auto idIt = typeIt->second.find(vote->vote->observedId);
        if (idIt == typeIt->second.end()) {
            idIt = typeIt->second.insert(std::make_pair(vote->vote->observedId, std::vector<VotedPosePtr>())).first;
        }

        idIt->second.push_back(vote);
        sources.insert(vote->source);
    }

    VotingBinResultPtr VotingBin::getResult(double sensitivity) {
        VotingBinResultPtr result;
        auto typeIt = votes.begin();
        auto typeItEnd = votes.end();
        for (; typeIt != typeItEnd; typeIt++) {
            auto idIt = typeIt->second.begin();
            auto idItEnd = typeIt->second.end();
            for (; idIt != idItEnd; idIt++) {
                auto voteIt = idIt->second.begin();
                auto voteItEnd = idIt->second.end();
                for (; voteIt != voteItEnd; voteIt++) {
                    this->fittingPose = (*voteIt)->pose;
                    this->currentType = typeIt;
                    this->currentId = idIt;
                    this->fittingStack = std::stack<VotedPosePtr>();
                    this->fittingStack.push(*voteIt);
                    this->takenSources.clear();
                    this->takenSources.insert((*voteIt)->source);
                    this->idealPoints.clear();
                    PointPtr projectedPoint = MathHelper::getOriginPoint(this->fittingPose,
                            (*voteIt)->vote->refToObjectQuat, (*voteIt)->vote->radius);
                    this->idealPoints.push_back(projectedPoint);

                    searchFit(sensitivity);
                    double confidence = 0;
                    ObjectSetPtr os(new ObjectSet());
                    while (!this->fittingStack.empty()) {
                        VotedPosePtr p = this->fittingStack.top();
                        this->fittingStack.pop();
                        confidence += p->weight;
                        ObjectPtr o(new Object(*(p->source)));
                        o->observedId = p->vote->observedId;
                        o->type = p->vote->objectType;
                        os->insert(o);
                    }

                    if (!result || (result && result->confidence < confidence)) {
                        result = VotingBinResultPtr(new VotingBinResult(
                                    os,
                                    this->fittingPose,
                                    confidence,
                                    std::vector<PointPtr>(this->idealPoints)));
                        if (os->objects.size() == sources.size()) {
                            return result;
                        }
                    }
                }
            }
        }

        return result;
    }

    void VotingBin::searchFit(double sensitivity) {
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

                for (; voteIt != voteItEnd; voteIt++) {
                    VotedPosePtr vote = *voteIt;

                    if (takenSources.find(vote->source) != takenSources.end()) {
                        continue;
                    }

                    PointPtr projectedPoint = MathHelper::getOriginPoint(fittingPose, vote->vote->refToObjectQuat,
                            vote->vote->radius);

                    double distance = MathHelper::getDistanceBetweenPoints(vote->source->pose->point, projectedPoint);
                    double angle = MathHelper::getAngleBetweenQuats(vote->pose->quat, fittingPose->quat);
                    if (distance <= sensitivity && angle < VB_MAX_PROJECTION_ANGLE_DEVIATION) {
                        idealPoints.push_back(projectedPoint);
                        takenSources.insert(vote->source);
                        fittingStack.push(vote);
                        break;
                    }
                }
            }
        }
    }
}

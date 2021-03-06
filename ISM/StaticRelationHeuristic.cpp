#include "StaticRelationHeuristic.hpp"

#include <vector>
#include "MathHelper.hpp"

namespace ISM {
    StaticRelationHeuristic::StaticRelationHeuristic(const TracksPtr& tracks) : Heuristic("StaticRelationHeuristic") {
        typedef MathHelper MH;

        for (auto& first : tracks->tracks) {
            TrackPtr currentBest;
            int bestStaticBreaks = 0;
            int bestCommonPositions = 0;
            double bestAvgDistance = 0;

            for (auto& second : tracks->tracks) {
                if (first == second) {
                    continue;
                }

                /*
                 * What we do:
                 * Calculate a vote from second to first (using first as the reference pose).
                 * Use that vote on the next frame to search first, looking from second.
                 * If the miss alignment is more than 10% of the average position difference between first and second,
                 * incease staticBreaks and recalculate the vote.
                 *
                 * At the end, if the staticBreaks are below 5% of the sample range,
                 * and they appear together in more than 95% of the frames,
                 * add second to the cluster with first as the reference point.
                 */

                VoteSpecifierPtr vote;
                int commonPositions = 0;
                int staticBreaks = 0;
                double averageDistance = 0;

                for (size_t i = 0; i < first->objects.size(); i++) {
                    auto firstObject = first->objects[i];
                    auto secondObject = second->objects[i];
                    if (!firstObject || !secondObject) {
                        continue;
                    }

                    averageDistance += MH::getDistanceBetweenPoints(
                        firstObject->pose->point,
                        secondObject->pose->point
                    );
                    commonPositions++;
                }

                averageDistance /= (double)commonPositions;

                double maxDeviation = averageDistance * 0.1;

                for (size_t i = 0; i < first->objects.size(); i++) {
                    auto firstObject = first->objects[i];
                    auto secondObject = second->objects[i];
                    if (!firstObject || !secondObject) {
                        continue;
                    }

                    if (!vote) {
                        vote = MH::getVoteSpecifierToPose(secondObject->pose, firstObject->pose);
                        continue;
                    }

                    auto referencePoint = MH::applyQuatAndRadiusToPose(
                        secondObject->pose,
                        vote->objectToRefQuat,
                        vote->radius
                    );

                    double distance = MH::getDistanceBetweenPoints(
                        firstObject->pose->point,
                        referencePoint
                    );

                    if (distance > maxDeviation) {
                        staticBreaks++;
                        vote = MH::getVoteSpecifierToPose(secondObject->pose, firstObject->pose);
                    }
                }

                if (
                    (double)staticBreaks < ((double)commonPositions) * 0.05 &&
                    commonPositions > (double)first->objects.size() * 0.5 &&
                    (!currentBest || staticBreaks < bestStaticBreaks ||
                            (staticBreaks == bestStaticBreaks && bestAvgDistance > averageDistance))
                ) {
                    currentBest = second;
                    bestStaticBreaks = staticBreaks;
                    bestCommonPositions = commonPositions;
                    bestAvgDistance = averageDistance;
                }
            }

            if (currentBest) {
                double conf = 1 - (double)bestStaticBreaks / (double)bestCommonPositions;
                if (conf > this->confidence) {
                    std::vector<TrackPtr> cluster;
                    cluster.push_back(first);
                    cluster.push_back(currentBest);
                    this->cluster = TracksPtr(new Tracks(cluster));
                    this->confidence = conf;
                }
            }
        }
    }
}

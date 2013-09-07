#include "DirectionOrientationRelationHeuristic.hpp"

#include <vector>
#include <boost/math/constants/constants.hpp>
#include <algorithm>
#include "MathHelper.hpp"

#define STATIC_BREAK_RATIO 0.01
#define TOGETHER_RATIO 0.90
#define MAX_ANGLE_DEVIATION 45

namespace ISM {
    DirectionOrientationRelationHeuristic::DirectionOrientationRelationHeuristic(const TracksPtr& tracks) :
            Heuristic("DirectionOrientationRelationHeuristic") {
        typedef MathHelper MH;
        typedef Eigen::Vector3d Vector;

        double bestDistance;
        for (auto& first : tracks->tracks) {
            TrackPtr currentBest;
            double currentClosestDistance;
            int currentBestBreaks;
            int currentBestCommonPositions;

            for (auto& second : tracks->tracks) {
                if (first == second) {
                    continue;
                }

                /*
                 * What we do:
                 * Calculate a direction Vector from first to second for every Frame.
                 * Check in every frame the angle between the reference vector (first vector) and the current vector.
                 * If the misalignment is more than MAX_ANGLE_DEVIATION degrees, increase staticBreaks and
                 * recalculate the reference vote.
                 * Also calculate the average distance between first and second.
                 *
                 * At the end, if the staticBreaks are below STATIC_BREAK_RATIO of the sample range,
                 * and they appear together in more than TOGETHER_RATIO of the frames,
                 * and the second is closer to first than the current closest track,
                 * replace the current closest track with second.
                 *
                 * At the end, choose the first<->second combination with the lowest rate of static breaks.
                 *
                 * This will always create a cluster of two tracks.
                 */

                int commonPositions = 0;
                double averageDistance = 0;

                for (size_t i = 0; i < first->objects.size(); i++) {
                    auto firstObject = first->objects[i];
                    auto secondObject = second->objects[i];
                    if (!firstObject || !secondObject) {
                        continue;
                    }

                    averageDistance += MH::getDistanceBetweenPoints(firstObject->pose->point,
                            secondObject->pose->point);
                    commonPositions++;
                }
                if (commonPositions < (double) first->objects.size() * TOGETHER_RATIO) {
                    continue;
                }

                averageDistance /= (double) commonPositions;

                int staticBreaks = 0;
                Vector directionVector;
                double orientationDifference;
                bool firstRun = true;

                for (size_t i = 0; i < first->objects.size(); i++) {
                    auto firstObject = first->objects[i];
                    auto secondObject = second->objects[i];
                    if (!firstObject || !secondObject) {
                        continue;
                    }

                    if (firstRun) {
                        directionVector = this->getDirectionVector(firstObject, secondObject);
                        orientationDifference = MH::getAngleBetweenQuats(
                            firstObject->pose->quat, secondObject->pose->quat
                        );
                        firstRun = false;
                        continue;
                    }

                    auto currentDirection = this->getDirectionVector(firstObject, secondObject);
                    auto currentOrientationDiff = MH::getAngleBetweenQuats(
                        firstObject->pose->quat, secondObject->pose->quat
                    );
                    auto deviation = MH::rad2deg(acos(directionVector.dot(currentDirection)));
                    auto orientationDeviation = fabs(orientationDifference - currentOrientationDiff);

                    if (deviation > MAX_ANGLE_DEVIATION || orientationDeviation > MAX_ANGLE_DEVIATION) {
                        staticBreaks++;
                        directionVector = currentDirection;
                        orientationDifference = currentOrientationDiff;
                    }
                }

                if (
                    ((double) staticBreaks < ((double) commonPositions) * STATIC_BREAK_RATIO) &&
                    (!currentBest || (currentClosestDistance > averageDistance))
                    ) {
                    currentBest = second;
                    currentClosestDistance = averageDistance;
                    currentBestBreaks = staticBreaks;
                    currentBestCommonPositions = commonPositions;
                }
            }

            if (currentBest) {
                double conf = 1 - (double) currentBestBreaks / (double) currentBestCommonPositions;
                if (!this->cluster
                    || (conf > this->confidence
                        || (conf == this->confidence && bestDistance > currentClosestDistance))) {
                    std::vector<TrackPtr> cluster;
                    cluster.push_back(first);
                    cluster.push_back(currentBest);
                    this->cluster = TracksPtr(new Tracks(cluster));
                    this->confidence = conf;
                    bestDistance = currentClosestDistance;
                }
            }
        }
    }

    Eigen::Vector3d DirectionOrientationRelationHeuristic::getDirectionVector(const ObjectPtr& first, const ObjectPtr& second) {
        typedef MathHelper MH;
        typedef Eigen::Vector3d Vector;

        auto firstToSecond = MH::pointToVector(second->pose->point) - MH::pointToVector(first->pose->point);
        auto firstRotation = MH::quatToEigenQuat(first->pose->quat);
        return firstRotation.inverse()._transformVector(firstToSecond).normalized();
    }
}

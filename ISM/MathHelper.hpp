#pragma once

#include <Eigen/Geometry>
#include "Pose.hpp"
#include "Quaternion.hpp"
#include "VoteSpecifier.hpp"

namespace ISM {
    class MathHelper {
        public:
            static VoteSpecifierPtr getVoteSpecifierToPoint(const PosePtr& pose, const PointPtr& refPoint);
            static PosePtr getReferencePose(const PosePtr& origin, const PointPtr& refPoint, const QuaternionPtr& refToOriginQuat);
            static PointPtr getOriginPoint(const PosePtr& refPose, const QuaternionPtr& refToObjectQuat, double radius);
            static PointPtr applyQuatAndRadiusToPose(const PosePtr& pose, const QuaternionPtr& quat, double radius);
            static Eigen::Vector3d applyQuatAndRadiusToPoseV(const PosePtr& pose, const QuaternionPtr& quat, double radius);
            static Eigen::Vector3d getViewportVector();
            static Eigen::Vector3d getPoseVectorFromQuat(const QuaternionPtr& quat);
            static Eigen::Vector3d pointToVector(const PointPtr& p);
            static Eigen::Quaternion<double> vectorRotationToEigenQuat(const Eigen::Vector3d& v1, const Eigen::Vector3d& v2);
            static PointPtr vectorToPoint(const Eigen::Vector3d& v);
            static Eigen::Quaternion<double> quatToEigenQuat(const QuaternionPtr& q);
            static QuaternionPtr eigenQuatToQuat(const Eigen::Quaternion<double>& q);
            static QuaternionPtr getAveragePose(const std::vector<QuaternionPtr>& poseQuats);
            static QuaternionPtr normalize(const QuaternionPtr& quat);
            static double getDistanceBetweenPoints(const PointPtr& p1, const PointPtr& p2);

            static double deg2rad(double deg);
            static double rad2deg(double rad);
    };
}

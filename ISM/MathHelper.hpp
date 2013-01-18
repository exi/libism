#pragma once

#include "Pose.hpp"
#include "Quaternion.hpp"
#include "VoteSpecifier.hpp"
#include <Eigen/Geometry>

namespace ISM {
    class MathHelper {
        public:
            static VoteSpecifierPtr getVoteSpecifierToPoint(PosePtr pose, PointPtr refPoint);
            static PosePtr getReferencePose(PosePtr origin, PointPtr refPoint, QuaternionPtr refToOriginQuat);
            static PointPtr applyQuatAndRadiusToPose(PosePtr pose, QuaternionPtr quat, double radius);
            static Eigen::Vector3d applyQuatAndRadiusToPoseV(PosePtr pose, QuaternionPtr quat, double radius);
            static Eigen::Vector3d getViewportVector();
            static Eigen::Vector3d getPoseVectorFromPose(PosePtr pose);
            static Eigen::Vector3d pointToVector(PointPtr p);
            static Eigen::Quaternion<double> vectorRotationToEigenQuat(Eigen::Vector3d v1, Eigen::Vector3d v2);
            static PointPtr vectorToPoint(Eigen::Vector3d v);
            static Eigen::Quaternion<double> quatToEigenQuat(QuaternionPtr q);
            static QuaternionPtr eigenQuatToQuat(Eigen::Quaternion<double> q);
    };
}

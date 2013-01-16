#pragma once

#include "Object.hpp"
#include "Quaternion.hpp"
#include "VoteSpecifier.hpp"
#include <Eigen/Geometry>

namespace ISM {
    class MathHelper {
        public:
            static VoteSpecifierPtr getVoteSpecifierToPoint(ObjectPtr o, PointPtr refPoint);
            static PointPtr projectVoteOntoPoint(VoteSpecifierPtr vote);
            static Eigen::Vector3d getViewportVector();
            static Eigen::Vector3d getPoseVectorFromObject(ObjectPtr o);
            static Eigen::Vector3d pointToVector(PointPtr p);
            static PointPtr vectorToPoint(Eigen::Vector3d v);
            static Eigen::Quaternion<double> quatToEigenQuat(QuaternionPtr q);
            static QuaternionPtr eigenQuatToQuat(Eigen::Quaternion<double> q);
    };
}

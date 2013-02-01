#include "MathHelper.hpp"

#include <boost/math/constants/constants.hpp>

typedef Eigen::Quaternion<double> EQuat;
typedef Eigen::Vector3d Vector;
namespace ISM {
    VoteSpecifierPtr MathHelper::getVoteSpecifierToPoint(const PosePtr& pose, const PointPtr& refPoint) {
        Vector objectPose = getPoseVectorFromQuat(pose->quat);
        Vector objectToPoint = pointToVector(refPoint) - pointToVector(pose->point);
        EQuat otp = vectorRotationToEigenQuat(objectPose, objectToPoint);
        EQuat pto = vectorRotationToEigenQuat(getViewportVector(), objectToPoint * -1.0);
        return VoteSpecifierPtr(
            new VoteSpecifier(
                eigenQuatToQuat(otp),
                eigenQuatToQuat(pto),
                objectToPoint.norm()
            )
        );
    }

    PosePtr MathHelper::getReferencePose(const PosePtr& origin, const PointPtr& refPoint, const QuaternionPtr& refToOriginQuat) {
        Vector pointToOrigin = pointToVector(origin->point) - pointToVector(refPoint);
        EQuat rotation = quatToEigenQuat(refToOriginQuat).inverse();
        Vector refPointPoseVector = rotation._transformVector(pointToOrigin);
        return PosePtr(
            new Pose(
                refPoint,
                eigenQuatToQuat(vectorRotationToEigenQuat(getViewportVector(), refPointPoseVector))
            )
        );
    }

    PointPtr MathHelper::applyQuatAndRadiusToPose(const PosePtr& pose, const QuaternionPtr& quat, double radius) {
        return vectorToPoint(applyQuatAndRadiusToPoseV(pose, quat, radius));
    }

    Vector MathHelper::applyQuatAndRadiusToPoseV(const PosePtr& pose, const QuaternionPtr& quat, double radius) {
        Vector objectPose = getPoseVectorFromQuat(pose->quat) * radius;
        Vector objectPoint = pointToVector(pose->point);
        EQuat rotation = quatToEigenQuat(quat);
        Vector projectionVector = rotation._transformVector(objectPose);
        return objectPoint + projectionVector;
    }

    Vector MathHelper::getPoseVectorFromQuat(const QuaternionPtr& quat) {
        EQuat rotation = quatToEigenQuat(quat);
        Vector viewport = getViewportVector();
        return rotation._transformVector(viewport);
    }

    Vector MathHelper::getViewportVector() {
        return Vector(1.0, 0.0, 0.0);
    }

    Vector MathHelper::pointToVector(const PointPtr& p) {
        return Vector(p->x, p->y, p->z);
    }

    EQuat MathHelper::vectorRotationToEigenQuat(const Vector& v1, const Vector& v2) {
        EQuat q;
        q.setFromTwoVectors(v1, v2);
        return q;
    }

    PointPtr MathHelper::vectorToPoint(const Vector& v) {
        return PointPtr(new Point(v[0], v[1], v[2]));
    }

    EQuat MathHelper::quatToEigenQuat(const QuaternionPtr& q) {
        EQuat ret(q->w, q->x, q->y, q->z);
        ret.normalize();
        return ret;
    }

    QuaternionPtr MathHelper::eigenQuatToQuat(const EQuat& q) {
        return QuaternionPtr(new Quaternion(q.w(), q.x(), q.y(), q.z()));
    }

    QuaternionPtr MathHelper::getAveragePose(const std::vector<QuaternionPtr>& poseQuats) {
        Vector combined(0, 0, 0);

        for (auto& quat : poseQuats) {
            combined += quatToEigenQuat(quat)._transformVector(getViewportVector());
        }

        EQuat ret;
        ret.setFromTwoVectors(getViewportVector(), combined);
        return eigenQuatToQuat(ret.normalized());
    }

    QuaternionPtr MathHelper::normalize(const QuaternionPtr& quat) {
        return eigenQuatToQuat(quatToEigenQuat(quat).normalized());
    }

    double MathHelper::deg2rad(double deg) {
        return deg * (boost::math::constants::pi<double>() / 180.0);
    }

    double MathHelper::rad2deg(double rad) {
        return rad * (180.0 / boost::math::constants::pi<double>());
    }
}

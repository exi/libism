#include "MathHelper.hpp"

#include <boost/math/constants/constants.hpp>

typedef Eigen::Quaternion<double> EQuat;
typedef Eigen::Vector3d Vector;
namespace ISM {
    VoteSpecifierPtr MathHelper::getVoteSpecifierToPoint(const PosePtr& pose, const PointPtr& refPoint) {
        //rotate everything relative to object pose
        Vector refVector = pointToVector(refPoint) - pointToVector(pose->point);
        EQuat p = quatToEigenQuat(pose->quat);
        Vector relativeRefPoint = p.inverse()._transformVector(refVector);
        Vector relativeRefPose = p.inverse()._transformVector(getViewportVector());

        EQuat otr = vectorRotationToEigenQuat(getViewportVector(), relativeRefPoint);
        EQuat otrp = vectorRotationToEigenQuat(getViewportVector(), relativeRefPose);

        //rotate everything relative to ref pose
        EQuat rto = vectorRotationToEigenQuat(getViewportVector(), refVector * -1.0);
        EQuat rtop = p;

        return VoteSpecifierPtr(
            new VoteSpecifier(
                eigenQuatToQuat(otr),
                eigenQuatToQuat(otrp),
                eigenQuatToQuat(rto),
                eigenQuatToQuat(rtop),
                refVector.norm()
            )
        );
    }

    PosePtr MathHelper::getReferencePose(const PosePtr& origin, const PointPtr& refPoint, const QuaternionPtr& refToOriginQuat) {
        //rotate everything so that origin is matching viewport axis, apply rotation to refPoint view, rotate back
        Vector refPoseObjRel = quatToEigenQuat(refToOriginQuat)._transformVector(getViewportVector());
        Vector refPoseGlobal = quatToEigenQuat(origin->quat)._transformVector(refPoseObjRel);
        return PosePtr(
            new Pose(
                refPoint,
                eigenQuatToQuat(vectorRotationToEigenQuat(getViewportVector(), refPoseGlobal))
            )
        );
    }

    PointPtr MathHelper::getOriginPoint(const PosePtr& refPose, const QuaternionPtr& refToObjectQuat, double radius) {
        Vector relativeObjectVector = quatToEigenQuat(refToObjectQuat)._transformVector(getViewportVector());
        Vector absoluteObjectVector = quatToEigenQuat(refPose->quat)._transformVector(relativeObjectVector);
        return vectorToPoint(pointToVector(refPose->point) + (absoluteObjectVector * radius));
    }

    PointPtr MathHelper::applyQuatAndRadiusToPose(const PosePtr& pose, const QuaternionPtr& quat, double radius) {
        return vectorToPoint(applyQuatAndRadiusToPoseV(pose, quat, radius));
    }

    Vector MathHelper::applyQuatAndRadiusToPoseV(const PosePtr& pose, const QuaternionPtr& quat, double radius) {
        //apply transformation to viewport vector, then scale, than transform to object coordinates and add
        EQuat rotation = quatToEigenQuat(quat);
        Vector projectionVector = rotation._transformVector(getViewportVector()) * radius;
        Vector projectionInGlobalSpace = quatToEigenQuat(pose->quat)._transformVector(projectionVector);

        Vector objectPoint = pointToVector(pose->point);
        return objectPoint + projectionInGlobalSpace;
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

    double MathHelper::getDistanceBetweenPoints(const PointPtr& p1, const PointPtr& p2) {
        return (pointToVector(p1) - pointToVector(p2)).norm();
    }

    double MathHelper::deg2rad(double deg) {
        return deg * (boost::math::constants::pi<double>() / 180.0);
    }

    double MathHelper::rad2deg(double rad) {
        return rad * (180.0 / boost::math::constants::pi<double>());
    }
}

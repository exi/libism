#include "MathHelper.hpp"

#include <boost/math/constants/constants.hpp>

typedef Eigen::Quaternion<double> EQuat;
typedef Eigen::Vector3d Vector;
namespace ISM {
    VoteSpecifierPtr MathHelper::getVoteSpecifierToPose(const PosePtr& pose, const PosePtr& refPose) {
        if ((pointToVector(refPose->point) - pointToVector(pose->point)).norm() == 0) {
            //avoid special case
            pose->point->x += 0.0000001;
        };
        Vector objToRefVector = pointToVector(refPose->point) - pointToVector(pose->point);
        Vector refToObjVector = objToRefVector * -1.0;
        EQuat p = quatToEigenQuat(pose->quat);
        EQuat r = quatToEigenQuat(refPose->quat);

        //rotate everything relative to object pose
        Vector relativeRefPoint = p.inverse()._transformVector(objToRefVector);

        EQuat otr = vectorRotationToEigenQuat(getViewportVector(), relativeRefPoint);
        EQuat otrp = (p.inverse()) * r;

        //rotate everything relative to ref pose
        Vector relativeObjPoint = r.inverse()._transformVector(refToObjVector);
        EQuat rto = vectorRotationToEigenQuat(getViewportVector(), relativeObjPoint);
        EQuat rtop = (r.inverse()) * p;

        return VoteSpecifierPtr(
            new VoteSpecifier(
                eigenQuatToQuat(otr),
                eigenQuatToQuat(otrp),
                eigenQuatToQuat(rto),
                eigenQuatToQuat(rtop),
                objToRefVector.norm()
            )
        );
    }

    PosePtr MathHelper::getReferencePose(const PosePtr& origin, const PointPtr& refPoint, const QuaternionPtr& objectToRefPoseQuat) {
        return PosePtr(
            new Pose(
                refPoint,
                eigenQuatToQuat(quatToEigenQuat(origin->quat) * quatToEigenQuat(objectToRefPoseQuat))
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
        EQuat rotation = quatToEigenQuat(pose->quat) * quatToEigenQuat(quat);
        Vector objectPoint = pointToVector(pose->point);
        return objectPoint + rotation._transformVector(getViewportVector()) * radius;
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

#include "MathHelper.hpp"

typedef Eigen::Quaternion<double> EQuat;
typedef Eigen::Vector3d Vector;
namespace ISM {
    VoteSpecifierPtr MathHelper::getVoteSpecifierToPoint(PosePtr pose, PointPtr refPoint) {
        Vector objectPose = getPoseVectorFromPose(pose);
        Vector objectToPoint = pointToVector(refPoint) - pointToVector(pose->point);
        std::cout<<"Object to point "<<vectorToPoint(objectToPoint * -1.0)<<std::endl;
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

    PosePtr MathHelper::getReferencePose(PosePtr origin, PointPtr refPoint, QuaternionPtr refToOriginQuat) {
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

    PointPtr MathHelper::applyQuatAndRadiusToPose(PosePtr pose, QuaternionPtr quat, double radius) {
        return vectorToPoint(applyQuatAndRadiusToPoseV(pose, quat, radius));
    }

    Vector MathHelper::applyQuatAndRadiusToPoseV(PosePtr pose, QuaternionPtr quat, double radius) {
        Vector objectPose = getPoseVectorFromPose(pose) * radius;
        Vector objectPoint = pointToVector(pose->point);
        EQuat rotation = quatToEigenQuat(quat);
        Vector projectionVector = rotation._transformVector(objectPose);
        return objectPoint + projectionVector;
    }

    Vector MathHelper::getPoseVectorFromPose(PosePtr pose) {
        EQuat rotation = quatToEigenQuat(pose->quat);
        Vector viewport = getViewportVector();
        return rotation._transformVector(viewport);
    }

    Vector MathHelper::getViewportVector() {
        return Vector(1.0, 0.0, 0.0);
    }

    Vector MathHelper::pointToVector(PointPtr p) {
        return Vector(p->x, p->y, p->z);
    }

    EQuat MathHelper::vectorRotationToEigenQuat(Vector v1, Vector v2) {
        EQuat q;
        q.setFromTwoVectors(v1, v2);
        return q;
    }

    PointPtr MathHelper::vectorToPoint(Vector v) {
        return PointPtr(new Point(v[0], v[1], v[2]));
    }

    EQuat MathHelper::quatToEigenQuat(QuaternionPtr q) {
        EQuat ret(q->w, q->x, q->y, q->z);
        ret.normalize();
        return ret;
    }

    QuaternionPtr MathHelper::eigenQuatToQuat(EQuat q) {
        return QuaternionPtr(new Quaternion(q.w(), q.x(), q.y(), q.z()));
    }
}

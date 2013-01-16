#include "MathHelper.hpp"

typedef Eigen::Quaternion<double> EQuat;
typedef Eigen::Vector3d Vector;
namespace ISM {
    VoteSpecifierPtr MathHelper::getVoteSpecifierToPoint(ObjectPtr o, PointPtr refPoint) {
        Vector objectPose = getPoseVectorFromObject(o);
        Vector objectPoint = pointToVector(o->getPose()->point);
        Vector objectToPoint = pointToVector(refPoint) - objectPoint;
        EQuat ret;
        ret.setFromTwoVectors(objectPose, objectToPoint);
        return VoteSpecifierPtr(new VoteSpecifier(eigenQuatToQuat(ret), objectToPoint.norm(), o));
    }

    PointPtr MathHelper::projectVoteOntoPoint(VoteSpecifierPtr vote) {
        Vector objectPose = getPoseVectorFromObject(vote->object) * vote->radius;
        Vector objectPoint = pointToVector(vote->object->getPose()->point);
        EQuat rotation = quatToEigenQuat(vote->quat);
        Vector projectionVector = rotation._transformVector(objectPose);
        return vectorToPoint(objectPoint + projectionVector);
    }

    Vector MathHelper::getPoseVectorFromObject(ObjectPtr o) {
        EQuat rotation = quatToEigenQuat(o->getPose()->quat);
        Vector viewport = getViewportVector();
        return rotation._transformVector(viewport);
    }

    Vector MathHelper::getViewportVector() {
        return Vector(1.0, 0.0, 0.0);
    }

    Vector MathHelper::pointToVector(PointPtr p) {
        return Vector(p->x, p->y, p->z);
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

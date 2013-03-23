#pragma once

#include "Object.hpp"
#include "Quaternion.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace ISM {
    struct VoteSpecifier {
        QuaternionPtr objectToRefQuat;
        QuaternionPtr objectToRefPoseQuat;
        QuaternionPtr refToObjectQuat;
        QuaternionPtr refToObjectPoseQuat;
        double radius;
        std::string patternName;
        std::string objectType;
        std::string observedId;
        VoteSpecifier(const VoteSpecifier& other): radius(other.radius),
                                                    patternName(other.patternName),
                                                    objectType(other.objectType),
                                                    observedId(other.observedId) {
            objectToRefQuat = QuaternionPtr(new Quaternion(*(other.objectToRefQuat)));
            objectToRefPoseQuat = QuaternionPtr(new Quaternion(*(other.objectToRefPoseQuat)));
            refToObjectQuat = QuaternionPtr(new Quaternion(*(other.refToObjectQuat)));
            refToObjectPoseQuat = QuaternionPtr(new Quaternion(*(other.refToObjectPoseQuat)));
        }

        VoteSpecifier(
                QuaternionPtr objectToRefQuat,
                QuaternionPtr objectToRefPoseQuat,
                QuaternionPtr refToObjectQuat,
                QuaternionPtr refToObjectPoseQuat,
                double radius
        ):
            objectToRefQuat(objectToRefQuat),
            objectToRefPoseQuat(objectToRefPoseQuat),
            refToObjectQuat(refToObjectQuat),
            refToObjectPoseQuat(refToObjectPoseQuat),
            radius(radius) {};

        VoteSpecifier(
                QuaternionPtr objectToRefQuat,
                QuaternionPtr objectToRefPoseQuat,
                QuaternionPtr refToObjectQuat,
                QuaternionPtr refToObjectPoseQuat,
                double radius,
                std::string patternName,
                std::string objectType,
                std::string observedId
        ):
            objectToRefQuat(objectToRefQuat),
            objectToRefPoseQuat(objectToRefPoseQuat),
            refToObjectQuat(refToObjectQuat),
            refToObjectPoseQuat(refToObjectPoseQuat),
            radius(radius),
            patternName(patternName),
            objectType(objectType),
            observedId(observedId) {};
    };
    typedef boost::shared_ptr<VoteSpecifier> VoteSpecifierPtr;
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifier &v);
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifierPtr &v);
}

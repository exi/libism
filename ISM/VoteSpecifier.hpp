#pragma once

#include "Object.hpp"
#include "Quaternion.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace ISM {
    struct VoteSpecifier {
        QuaternionPtr objectToRefQuat;
        QuaternionPtr refToObjectQuat;
        double radius;
        std::string patternName;
        std::string objectType;
        std::string observedId;
        VoteSpecifier(const VoteSpecifier& other): radius(other.radius),
                                                    patternName(other.patternName),
                                                    objectType(other.objectType),
                                                    observedId(other.observedId) {
            objectToRefQuat = QuaternionPtr(new Quaternion(*(other.objectToRefQuat)));
            refToObjectQuat = QuaternionPtr(new Quaternion(*(other.refToObjectQuat)));
        }
        VoteSpecifier(QuaternionPtr objectToRefQuat, QuaternionPtr refToObjectQuat, double radius):
            objectToRefQuat(objectToRefQuat), refToObjectQuat(refToObjectQuat), radius(radius) {};
        VoteSpecifier(
                QuaternionPtr objectToRefQuat,
                QuaternionPtr refToObjectQuat,
                double radius,
                std::string patternName,
                std::string objectType,
                std::string observedId):
            objectToRefQuat(objectToRefQuat),
            refToObjectQuat(refToObjectQuat),
            radius(radius),
            patternName(patternName),
            objectType(objectType),
            observedId(observedId) {};
    };
    typedef boost::shared_ptr<VoteSpecifier> VoteSpecifierPtr;
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifier &v);
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifierPtr &v);
}

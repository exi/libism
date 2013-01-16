#pragma once

#include "Object.hpp"
#include "Quaternion.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

namespace ISM {
    struct VoteSpecifier {
        QuaternionPtr quat;
        double radius;
        ObjectPtr object;
        std::string patternName;
        VoteSpecifier(QuaternionPtr quat, double radius, ObjectPtr object):
            quat(quat), radius(radius), object(object) {};

        void setPatternName(std::string patternName) {
            this->patternName = patternName;
        }
    };
    typedef boost::shared_ptr<VoteSpecifier> VoteSpecifierPtr;
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifier &v);
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifierPtr &v);
}

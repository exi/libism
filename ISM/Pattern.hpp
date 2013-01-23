#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

namespace ISM {
    struct Pattern {
        std::string name;
        int expectedObjectCount;
        double referencePointSpread;
        Pattern(std::string name, int expectedObjectCount, double referencePointSpread):
            name(name), expectedObjectCount(expectedObjectCount), referencePointSpread(referencePointSpread) {};
    };

    typedef boost::shared_ptr<Pattern> PatternPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Pattern &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::PatternPtr &o);
}

#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

namespace ISM {
    struct Pattern {
        std::string name;
        int expectedMaxWeight;
        double referencePointSpread;
        Pattern(std::string name, int expectedMaxWeight, double referencePointSpread):
            name(name), expectedMaxWeight(expectedMaxWeight), referencePointSpread(referencePointSpread) {};
    };

    typedef boost::shared_ptr<Pattern> PatternPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Pattern &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::PatternPtr &o);
}

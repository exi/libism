#pragma once

#include <boost/shared_ptr.hpp>
#include <string>

namespace ISM {
    struct Pattern {
        std::string name;
        int expectedMaxWeight;
        Pattern(std::string name, int expectedMaxWeight):
            name(name), expectedMaxWeight(expectedMaxWeight) {};
    };

    typedef boost::shared_ptr<Pattern> PatternPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Pattern &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::PatternPtr &o);
}

#include "VoteSpecifier.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifier &v) {
        return strm<<"vote: ["<<
            v.patternName<<";"<<
            v.objectType<<";"<<
            v.observedId<<";"<<
            v.radius<<";"<<
            v.objectToRefQuat<<";"<<
            v.refToObjectQuat<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifierPtr &v) {
        return strm<<(*v);
    }
}

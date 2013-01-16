#include "VoteSpecifier.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifier &v) {
        return strm<<"vote: ["<<v.patternName<<";"<<v.radius<<";"<<v.quat<<";"<<v.object<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::VoteSpecifierPtr &v) {
        return strm<<(*v);
    }
}

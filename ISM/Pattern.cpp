#include "Pattern.hpp"

namespace ISM{
    std::ostream& operator<<(std::ostream &strm, const ISM::Pattern &p) {
        return strm<<"pattern: ["<<p.name<<";"<<p.expectedMaxWeight<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::PatternPtr &p) {
        return strm<<(*p);
    }
}

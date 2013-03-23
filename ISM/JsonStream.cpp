#include "JsonStream.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::json &s) {
        s.s->serialize(strm);
        return strm;
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::jsonPtr &s) {
        return strm<<(*s);
    }
}

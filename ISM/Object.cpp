#include "Object.hpp"

namespace ISM{
    std::ostream& operator<<(std::ostream &strm, const ISM::Object &o) {
        return strm<<"object: ["<<o.type<<";"<<o.observedId<<";"<<o.pose<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectPtr &o) {
        return strm<<(*o);
    }
}

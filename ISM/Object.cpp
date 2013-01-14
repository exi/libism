#include "Object.hpp"

namespace ISM{
    std::ostream& operator<<(std::ostream &strm, const ISM::Object &o) {
        return strm<<"object: "<<o.getType()<<", "<<o.getID()<<", "<<o.getPose();
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectPtr &o) {
        return strm<<"object: "<<o->getType()<<", "<<o->getID()<<", "<<o->getPose();
    }
}

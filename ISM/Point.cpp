#include "Point.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Point &p) {
        return strm<<"["<<p.x<<","<<p.y<<","<<p.z<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::PointPtr &p) {
        return strm<<"["<<p->x<<","<<p->y<<","<<p->z<<"]";
    }
}

#include "Point.hpp"
#include "JsonStream.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Point &p) {
        return strm<<"point: ["<<p.x<<";"<<p.y<<";"<<p.z<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::PointPtr &p) {
        return strm<<(*p);
    }

    void Point::serialize(std::ostream& strm) const {
        strm<<"{\"x\": "<<this->x<<", \"y\": "<<this->y<<", \"z\": "<<this->z<<"}";
    }
}

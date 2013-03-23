#include "Quaternion.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Quaternion &q) {
        return strm<<"quaternion: ["<<q.w<<";"<<q.x<<";"<<q.y<<";"<<q.z<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::QuaternionPtr &q) {
        return strm<<(*q);
    }

    void Quaternion::serialize(std::ostream& strm) const {
        strm<<"{\"w\": "<<this->w<<", \"x\": "<<this->x<<", \"y\": "<<this->y<<", \"z\": "<<this->z<<"}";
    }
}

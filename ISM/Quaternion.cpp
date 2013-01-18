#include "Quaternion.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Quaternion &q) {
        return strm<<"quaternion: ["<<q.w<<";"<<q.x<<";"<<q.y<<";"<<q.z<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::QuaternionPtr &q) {
        return strm<<(*q);
    }
}

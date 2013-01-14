#include "Quaternion.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Quaternion &q) {
        return strm<<"["<<q.x<<","<<q.y<<","<<q.z<<","<<q.w<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::QuaternionPtr &q) {
        return strm<<"["<<q->x<<","<<q->y<<","<<q->z<<","<<q->w<<"]";
    }
}

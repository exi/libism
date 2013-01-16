#include "Pose.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Pose &p) {
        return strm<<"pose: ["<<p.point<<";"<<p.quat<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::PosePtr &p) {
        return strm<<(*p);
    }
}

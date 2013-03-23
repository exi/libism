#include "Pose.hpp"
#include "JsonStream.hpp"
#include "MathHelper.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::Pose &p) {
        return strm<<"pose: ["<<p.point<<";"<<p.quat<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::PosePtr &p) {
        return strm<<(*p);
    }

    void Pose::serialize(std::ostream& strm) const {
        strm<<"{"<<std::endl<<"\"point\": "<<json(this->point)<<", "<<std::endl
            <<"\"quaternion\": "<<json(this->quat)<<", "<<std::endl
            <<"\"poseVector\": "<<json(MathHelper::vectorToPoint(MathHelper::getPoseVectorFromQuat(this->quat)))
            <<std::endl<<"}";
    }
}

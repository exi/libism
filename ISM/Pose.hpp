#pragma once

#include <sstream>
#include <string>
#include <ostream>
#include "Quaternion.hpp"
#include "Point.hpp"

namespace ISM {
    class Pose {
        public:
            Pose() {
                this->point.reset(new Point(0, 0, 0));
                this->quat.reset(new Quaternion(0, 0, 0, 0));
            };
            Pose(Point *p, Quaternion* q) {
                this->point.reset(p);
                this->quat.reset(q);
            };
            Pose(PointPtr p, QuaternionPtr quat):point(p), quat(quat) {};
            PointPtr point;
            QuaternionPtr quat;
    };
    typedef boost::shared_ptr<Pose> PosePtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Pose &p);
    std::ostream& operator<<(std::ostream &strm, const ISM::PosePtr &p);
}

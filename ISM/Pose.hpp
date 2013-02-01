#pragma once

#include <sstream>
#include <string>
#include <ostream>
#include "Quaternion.hpp"
#include "Point.hpp"

namespace ISM {
    class Pose {
        public:
            PointPtr point;
            QuaternionPtr quat;

            Pose() {
                this->point.reset(new Point(0, 0, 0));
                this->quat.reset(new Quaternion(0, 0, 0, 0));
            };

            Pose(const Pose& other) {
                this->point.reset(new Point(*(other.point)));
                this->quat.reset(new Quaternion(*(other.quat)));
            }

            Pose(Point *p, Quaternion* q) {
                this->point.reset(p);
                this->quat.reset(q);
            };

            Pose(PointPtr p, QuaternionPtr quat): point(p), quat(quat) {};
    };
    typedef boost::shared_ptr<Pose> PosePtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Pose &p);
    std::ostream& operator<<(std::ostream &strm, const ISM::PosePtr &p);
}

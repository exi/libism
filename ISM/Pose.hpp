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
                this->point = PointPtr(new Point(0, 0, 0));
                this->quat = QuaternionPtr(new Quaternion(0, 0, 0, 0));
            };

            Pose(const Pose& other) {
                this->point = PointPtr(new Point(*(other.point)));
                this->quat = QuaternionPtr(new Quaternion(*(other.quat)));
            }

            Pose(Point *p, Quaternion* q) {
                this->point = PointPtr(p);
                this->quat = QuaternionPtr(q);
            };

            Pose(PointPtr p, QuaternionPtr quat): point(p), quat(quat) {};
    };
    typedef boost::shared_ptr<Pose> PosePtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Pose &p);
    std::ostream& operator<<(std::ostream &strm, const ISM::PosePtr &p);
}

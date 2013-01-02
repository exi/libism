#pragma once

namespace ISM {
    class Point {
        public:
            Point(): x(0), y(0), z(0) { }
            Point(double x, double y, double z) : x(x), y(y), z(z) {};
            double x;
            double y;
            double z;
    };

    class Quaternion {
        public:
                Quaternion(): x(0), y(0), z(0), w(0) { }
            Quaternion(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {};
            double x;
            double y;
            double z;
            double w;
    };

    class Pose {
        public:
            Pose() {
                this->point = Point(0, 0, 0);
                this->quat = Quaternion(0, 0, 0, 0);
            };
            Pose(Point p, Quaternion quat):point(p), quat(quat) {};
            Point point;
            Quaternion quat;
    };
}

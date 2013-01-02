#pragma once

namespace ISM {
    class Point {
        public:
            Point(double x, double y, double z) : x(x), y(y), z(z) {};
            double x;
            double y;
            double z;
    };

    class Quaternion {
        public:
            Quaternion(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {};
            double x;
            double y;
            double z;
            double w;
    };

    class Pose {
        public:
            Pose(Point p, Quaternion quat):point(p), quat(quat) {};
            Point point;
            Quaternion quat;
    };
}

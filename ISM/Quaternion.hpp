#pragma once

#include <boost/shared_ptr.hpp>
namespace ISM {
    class Quaternion {
        public:
                Quaternion(): x(0), y(0), z(0), w(0) { }
            Quaternion(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {};
            double x;
            double y;
            double z;
            double w;
    };
    typedef boost::shared_ptr<Quaternion> QuaternionPtr;
    std::ostream& operator<<(std::ostream &strm, const ISM::Quaternion &q);
    std::ostream& operator<<(std::ostream &strm, const ISM::QuaternionPtr &q);
}

#pragma once

#include <boost/shared_ptr.hpp>
#include "Serializable.hpp"

namespace ISM {
    class Quaternion : public Serializable {
        public:
            Quaternion(): w(0), x(0), y(0), z(0) { }
            Quaternion(double w, double x, double y, double z) : w(w), x(x), y(y), z(z) {};
            double w;
            double x;
            double y;
            double z;

            virtual void serialize(std::ostream& strm) const;
    };
    typedef boost::shared_ptr<Quaternion> QuaternionPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Quaternion &q);
    std::ostream& operator<<(std::ostream &strm, const ISM::QuaternionPtr &q);
}

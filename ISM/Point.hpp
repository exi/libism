#pragma once

#include <boost/shared_ptr.hpp>
#include "Serializable.hpp"

namespace ISM {
    class Point : public Serializable {
        public:
            Point(): x(0), y(0), z(0) { }
            Point(double x, double y, double z) : x(x), y(y), z(z) {};
            double x;
            double y;
            double z;

            virtual void serialize(std::ostream& strm) const;
    };
    typedef boost::shared_ptr<Point> PointPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Point &p);
    std::ostream& operator<<(std::ostream &strm, const ISM::PointPtr &p);
}

#pragma once

#include <boost/shared_ptr.hpp>
#include <algorithm>
#include "Point.hpp"

namespace ISM {
    class MinMaxFinder {
        public:
            double minX, maxX;
            double minY, maxY;
            double minZ, maxZ;

            MinMaxFinder() {
                this->maxX = -std::numeric_limits<double>::max();
                this->minX = std::numeric_limits<double>::max();
                this->maxY = -std::numeric_limits<double>::max();
                this->minY = std::numeric_limits<double>::max();
                this->maxZ = -std::numeric_limits<double>::max();
                this->minZ = std::numeric_limits<double>::max();
            };

            void add(double x, double y, double z) {
                this->maxX = std::max(x, this->maxX);
                this->minX = std::min(x, this->minX);
                this->maxY = std::max(y, this->maxY);
                this->minY = std::min(y, this->minY);
                this->maxZ = std::max(z, this->maxZ);
                this->minZ = std::min(z, this->minZ);
            };

            double getMaxSpread() const {
                using namespace std;
                return max(this->maxX - this->minX, max(this->maxY - this->minY, max(this->maxZ, this->minZ)));
            };

            PointPtr getMiddlePoint() const {
                return PointPtr(
                    new Point(
                        (this->minX + this->maxX) / 2.0,
                        (this->minY + this->maxY) / 2.0,
                        (this->minZ + this->maxZ) / 2.0
                    )
                );
            };
    };

    typedef boost::shared_ptr<MinMaxFinder> MinMaxFinderPtr;
}

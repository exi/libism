#pragma once

#include <boost/shared_ptr.hpp>
#include <algorithm>

namespace ISM {
    class MinMaxFinder {
        public:
            double minX, maxX;
            double minY, maxY;
            double minZ, maxZ;

            MinMaxFinder() {
                this->maxX = std::numeric_limits<double>::min();
                this->minX = std::numeric_limits<double>::max();
                this->maxY = std::numeric_limits<double>::min();
                this->minY = std::numeric_limits<double>::max();
                this->maxZ = std::numeric_limits<double>::min();
                this->minZ = std::numeric_limits<double>::max();
            }

            void add(double x, double y, double z) {
                this->maxX = std::max(x, this->maxX);
                this->minX = std::min(x, this->minX);
                this->maxY = std::max(y, this->maxY);
                this->minY = std::min(y, this->minY);
                this->maxZ = std::max(z, this->maxZ);
                this->minZ = std::min(z, this->minZ);
            }

            double getMaxSpread() {
                using namespace std;
                return max(this->maxX - this->minX, max(this->maxY - this->minY, max(this->maxZ, this->minZ)));
            }
    };

    typedef boost::shared_ptr<MinMaxFinder> MinMaxFinderPtr;
}

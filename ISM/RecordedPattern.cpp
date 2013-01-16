#include "RecordedPattern.hpp"

namespace ISM {
    void RecordedPattern::addObjectSet(ObjectSetPtr os) {
        PointPtr p = os->getReferencePoint();
        this->x += p->x;
        this->y += p->y;
        this->z += p->z;
        this->objectSets.push_back(os);
    }

    PointPtr RecordedPattern::getAbsoluteReferencePoint() {
        double count = (double)this->objectSets.size();
        return PointPtr(new Point(this->x / count, this->y / count, this->z / count));
    }
}

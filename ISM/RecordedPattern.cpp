#include "RecordedPattern.hpp"

namespace ISM {
    void RecordedPattern::addObjectSet(ObjectSetPtr os) {
        PointPtr p = os->getReferencePoint();
        this->x += p->x;
        this->y += p->y;
        this->z += p->z;
        this->objectSets.push_back(os);
        this->minMaxFinder->add(p->x, p->y, p->z);
    }

    PointPtr RecordedPattern::getAbsoluteReferencePoint() {
        double count = this->objectSets.size();
        return PointPtr(new Point(this->x / count, this->y / count, this->z / count));
    }
}

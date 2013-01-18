#include "ObjectSet.hpp"

namespace ISM {
    void ObjectSet::insert(ObjectPtr o) {
        objects.push_back(o);
    }

    PointPtr ObjectSet::getReferencePoint() {
        MinMaxFinder m;
        BOOST_FOREACH(ObjectPtr o, this->objects) {
            PointPtr p = o->pose->point;
            m.add(p->x, p->y, p->z);
        }

        return PointPtr(new Point((m.minX + m.maxX) / 2.0, (m.minY + m.maxY) / 2.0, (m.minZ + m.maxZ) / 2.0));
    };
}

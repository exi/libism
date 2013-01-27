#include "ObjectSet.hpp"

namespace ISM {
    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectSet &o) {
        strm<<"object set: ["<<std::endl;
        for (auto& object : o.objects) {
            strm<<object<<std::endl;
        };
        return strm<<"]";
    }

    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectSetPtr &o) {
        return strm<<(*o);
    }

    void ObjectSet::insert(const ObjectPtr& o) {
        objects.push_back(o);
    }

    const PointPtr ObjectSet::getReferencePoint() const {
        MinMaxFinder m;
        BOOST_FOREACH(ObjectPtr o, this->objects) {
            PointPtr p = o->pose->point;
            m.add(p->x, p->y, p->z);
        }

        return PointPtr(new Point((m.minX + m.maxX) / 2.0, (m.minY + m.maxY) / 2.0, (m.minZ + m.maxZ) / 2.0));
    }
}

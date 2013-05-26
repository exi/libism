#include "ObjectSet.hpp"
#include "MathHelper.hpp"

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

    void ObjectSet::insert(ObjectPtr o) {
        o->pose->quat = MathHelper::normalize(o->pose->quat);
        objects.push_back(o);
    }
}

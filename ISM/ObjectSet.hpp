#pragma once

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include "Object.hpp"
#include "Point.hpp"
#include "MinMaxFinder.hpp"

namespace ISM {
    class ObjectSet {
        public:
            std::vector<ObjectPtr> objects;

            ObjectSet() {};

            void insert(const ObjectPtr& o);

            const PointPtr getReferencePoint() const;
    };

    typedef boost::shared_ptr<ObjectSet> ObjectSetPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectSet &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectSetPtr &o);
}

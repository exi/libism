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

            void insert(ObjectPtr o);

            PointPtr getReferencePoint();
    };
    typedef boost::shared_ptr<ObjectSet> ObjectSetPtr;
}

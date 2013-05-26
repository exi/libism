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
            ObjectSet(const ObjectSet& other) {
                for (auto& objectptr : other.objects) {
                    this->objects.push_back(ObjectPtr(new Object(*objectptr)));
                }
            }

            void insert(ObjectPtr o);
    };

    typedef boost::shared_ptr<ObjectSet> ObjectSetPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectSet &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectSetPtr &o);
}

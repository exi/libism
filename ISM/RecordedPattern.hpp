#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Point.hpp"
#include "ObjectSet.hpp"

namespace ISM {
    class RecordedPattern {
        public:
            std::string name;
            std::vector<ObjectSetPtr> objectSets;
            double x, y, z;

            RecordedPattern(std::string name): name(name), x(0), y(0), z(0) { };

            void addObjectSet(ObjectSetPtr os);
            std::vector<ObjectSetPtr> getObjectSets() { return this->objectSets; };
            std::string getName() { return this->name; };
            PointPtr getAbsoluteReferencePoint();
    };
    typedef boost::shared_ptr<RecordedPattern> RecordedPatternPtr;
}

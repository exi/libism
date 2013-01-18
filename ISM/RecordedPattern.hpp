#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

#include "Point.hpp"
#include "ObjectSet.hpp"
#include "MinMaxFinder.hpp"

namespace ISM {
    class RecordedPattern {
        public:
            std::string name;
            std::vector<ObjectSetPtr> objectSets;
            double x, y, z;
            MinMaxFinderPtr minMaxFinder;

            RecordedPattern(std::string name): name(name), x(0), y(0), z(0) {
                this->minMaxFinder.reset(new MinMaxFinder());
            };

            void addObjectSet(ObjectSetPtr os);
            PointPtr getAbsoluteReferencePoint();
    };
    typedef boost::shared_ptr<RecordedPattern> RecordedPatternPtr;
}

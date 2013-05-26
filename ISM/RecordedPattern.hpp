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

            RecordedPattern(std::string name): name(name) { };

            void addObjectSet(ObjectSetPtr os);
    };
    typedef boost::shared_ptr<RecordedPattern> RecordedPatternPtr;
}

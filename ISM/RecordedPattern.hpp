#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace ISM {
    class RecordedPattern {
        public:
            std::string name;
            std::vector<ObjectSetPtr> objectSets;

            RecordedPattern(std::string name):name(name) { };

            void addObjectSet(ObjectSetPtr os) { this->objectSets.push_back(os); };
            std::vector<ObjectSetPtr> getObjectSets() { return this->objectSets; };
            std::string getName() { return this->name; };
    };
    typedef boost::shared_ptr<RecordedPattern> RecordedPatternPtr;
}

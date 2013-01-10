#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

namespace ISM {
    class RecordedPattern {
        public:
            std::string name;
            std::vector<boost::shared_ptr<ObjectSet> > objectSets;

            RecordedPattern(std::string name):name(name) { };

            void addObjectSet(boost::shared_ptr<ObjectSet> os) { this->objectSets.push_back(os); };
            std::vector<boost::shared_ptr<ObjectSet> > getObjectSets() { return this->objectSets; };
            std::string getName() { return this->name; };
    };
}

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

            void addObjectSet(boot::shared_ptr<ObjectSet> os) {this->objectSets.push_back(os)};
    };
}

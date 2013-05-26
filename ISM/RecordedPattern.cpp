#include "RecordedPattern.hpp"

namespace ISM {
    void RecordedPattern::addObjectSet(ObjectSetPtr os) {
        this->objectSets.push_back(os);
    }
}

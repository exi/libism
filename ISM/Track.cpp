#include "Tracks.hpp"
#include <math.h>

namespace ISM {
    void Track::calculateWeight() {
        double sum = 0;
        int count = 0;
        for (auto& obj : this->objects) {
            if (obj) {
                sum += obj->weight;
                count++;
            }
        }

        this->weight = floor((sum / (double)count) + 0.5);
    }
}

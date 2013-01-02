#pragma once

#include "ObjectSet.hpp"
#include "Object.hpp"

namespace ISM {
    class Recorder {
        public:
            ObjectSet s;
            void insert(ISM::Object* o) {
                s.insert(o);
            }
    };
}

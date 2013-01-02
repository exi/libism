#pragma once

#include <vector>
#include <iostream>
#include "Object.hpp"

namespace ISM {
    class ObjectSet {
        public:
            std::vector<ISM::Object*> objects;
            void insert(ISM::Object* o) {
                objects.push_back(o);
                std::cout<<"insert object"<<o->toString()<<std::endl;
            }
    };
}

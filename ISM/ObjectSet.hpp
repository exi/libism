#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "Object.hpp"
#include <boost/shared_ptr.hpp>

namespace ISM {
    class ObjectSet {
        public:
            std::vector<boost::shared_ptr<Object> > objects;

            void insert(boost::shared_ptr<Object> o) {
                objects.push_back(o);
                std::cout<<"insert "<<o->toString()<<std::endl;
            }

            std::vector<boost::shared_ptr<Object> > getObjects() { return this->objects; };
    };
}

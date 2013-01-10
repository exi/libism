#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "Object.hpp"
#include <boost/shared_ptr.hpp>

namespace ISM {
    class ObjectSet {
        public:
            std::string patternName;
            std::vector<boost::shared_ptr<Object> > objects;

            ObjectSet() {};
            ~ObjectSet() {
                std::cout<<"ObjectSet "<<this->patternName<<" destroyed"<<std::endl;
            }

            ObjectSet(std::string patternName): patternName(patternName) {};
            void insert(boost::shared_ptr<Object> o) {
                objects.push_back(o);
                std::cout<<"insert "<<o->toString()<<std::endl;
            }

            void setPatternName(std::string patternName) { this->patternName = patternName; };

            std::vector<boost::shared_ptr<Object> > getObjects() { return this->objects; };
            std::string getPatternName() { return this->patternName; };
    };
}

#pragma once

#include <vector>
#include <iostream>
#include <string>
#include "Object.hpp"
#include <boost/shared_ptr.hpp>

namespace ISM {
    class ObjectSet {
        public:
            std::string sceneName;
            std::vector<boost::shared_ptr<Object> > objects;

            ObjectSet() {};
            ~ObjectSet() {
                std::cout<<"ObjectSet "<<this->sceneName<<" destroyed"<<std::endl;
            }

            ObjectSet(std::string sceneName): sceneName(sceneName) {};
            void insert(boost::shared_ptr<Object> o) {
                objects.push_back(o);
                std::cout<<"insert "<<o->toString()<<std::endl;
            }

            void setSceneName(std::string sceneName) { this->sceneName = sceneName; };

            std::vector<boost::shared_ptr<Object> > getObjects() { return this->objects; };
            std::string getSceneName() { return this->sceneName; };
    };
}

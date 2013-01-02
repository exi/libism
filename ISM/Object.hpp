#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "Pose.hpp"

namespace ISM {
    class Object {
        public:
            Pose pose;
            std::string id;

            virtual ~Object() {
                std::cout<<"Object "<<this->id<<" destroyed"<<std::endl;
            }

            virtual void setPose(Pose p) { this->pose = p; };
            virtual void setID(std::string id) { this->id = id; };
            virtual Pose getPose() { return this->pose; };
            virtual std::string getID() { return this->id; };
            virtual std::string toString() {
                std::stringstream str;
                str<<"object: "<<this->id<<" "<<this->pose.point.x;
                return str.str();
            }
    };

    class ObservedObject: public Object {
        public:
            virtual ~ObservedObject() {
                std::cout<<"ObservedObject "<<this->id<<" destroyed"<<std::endl;
            }
    };
}

#pragma once

#include <string>
#include <sstream>
#include "Pose.hpp"

namespace ISM {
    class Object {
        public:
            Pose pose;
            std::string id;
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
    };
}

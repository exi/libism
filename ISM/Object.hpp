#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "Pose.hpp"
#include <boost/shared_ptr.hpp>

namespace ISM {
    class Object {
        public:
            std::string id;
            std::string type;
            PosePtr pose;

            Object(): id(""), type("") {};
            Object(std::string id, std::string type, PosePtr pose): id(id), type(type), pose(pose) {};
            Object(std::string id, std::string type, Pose* pose): id(id), type(type) {
                this->pose.reset(pose);
            };

            virtual ~Object() {
                std::cout<<"Object "<<this->id<<" destroyed"<<std::endl;
            }

            void setPose(PosePtr p) { this->pose = p; };
            void setID(std::string id) { this->id = id; };
            void setType(std::string type) { this->type = type; };
            PosePtr getPose() const { return this->pose; };
            std::string getID() const { return this->id; };
            std::string getType() const { return this->type; };


    };

    typedef boost::shared_ptr<Object> ObjectPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Object &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectPtr &o);
}

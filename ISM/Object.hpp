#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "Pose.hpp"

namespace ISM {
    class Object {
        public:
            std::string id;
            std::string type;
            Pose pose;

            Object(): id(""), type("") {};
            Object(std::string id, std::string type, Pose pose): id(id), type(type), pose(pose) {};

            virtual ~Object() {
                std::cout<<"Object "<<this->id<<" destroyed"<<std::endl;
            }

            virtual void setPose(Pose p) { this->pose = p; };
            virtual void setID(std::string id) { this->id = id; };
            virtual void setType(std::string type) { this->type = type; };
            virtual Pose getPose() { return this->pose; };
            virtual std::string getID() { return this->id; };
            virtual std::string getType() { return this->type; };
            virtual std::string toString() {
                std::stringstream str;
                str<<"object: "<<
                    this->type<<", "<<
                    this->id<<", ["<<
                    this->pose.point.x<<", "<<
                    this->pose.point.y<<", "<<
                    this->pose.point.z<<"], ["<<
                    this->pose.quat.x<<", "<<
                    this->pose.quat.y<<", "<<
                    this->pose.quat.z<<", "<<
                    this->pose.quat.w<<"]";
                return str.str();
            }
    };
}

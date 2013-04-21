#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include "Pose.hpp"
#include <boost/shared_ptr.hpp>
#include "Serializable.hpp"

namespace ISM {
    class Object : public Serializable {
        public:
            std::string type;
            std::string observedId;
            PosePtr pose;
            double confidence;

            Object(const Object& other): type(other.type), observedId(other.observedId), confidence(other.confidence) {
                this->pose = PosePtr(new Pose(*other.pose));
            };

            Object(std::string type, PosePtr pose, std::string observedId = ""): type(type), observedId(observedId), pose(pose), confidence(1) {};

            Object(std::string type, Pose* pose, std::string observedId = ""): type(type), observedId(observedId), confidence(1) {
                this->pose = PosePtr(pose);
            };

            virtual void serialize(std::ostream& strm) const;
    };

    typedef boost::shared_ptr<Object> ObjectPtr;

    std::ostream& operator<<(std::ostream &strm, const ISM::Object &o);
    std::ostream& operator<<(std::ostream &strm, const ISM::ObjectPtr &o);
}

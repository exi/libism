#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "Object.hpp"

namespace ISM {
    struct Track {
        std::string type;
        std::string observedId;
        std::vector<ObjectPtr> objects;
        double weight;

        Track(std::string type = "", std::string observedId = "") : type(type), observedId(observedId), weight(1.0) {};

        void calculateWeight();
    };

    typedef boost::shared_ptr<Track> TrackPtr;
}

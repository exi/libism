#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include "Tracks.hpp"

namespace ISM {
    class Heuristic {
        protected:
            Heuristic(std::string name) : name(name) {};
        public:
            std::string name;
            double confidence = 0;
            TracksPtr cluster;
    };

    typedef boost::shared_ptr<Heuristic> HeuristicPtr;
}

#pragma once

#include <boost/shared_ptr.hpp>
#include <vector>
#include <string>
#include "Tracks.hpp"

namespace ISM {
    class Heuristic {
        protected:
            Heuristic() : confidence(0) {}
            Heuristic(std::string name) : name(name), confidence(0) {};
        public:
            std::string name;
            double confidence;
            TracksPtr cluster;
    };

    typedef boost::shared_ptr<Heuristic> HeuristicPtr;
}

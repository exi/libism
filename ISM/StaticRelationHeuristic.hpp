#pragma once

#include <boost/shared_ptr.hpp>
#include "Tracks.hpp"
#include "Heuristic.hpp"

namespace ISM {
    class StaticRelationHeuristic: public Heuristic {
        public:
            StaticRelationHeuristic(TracksPtr tracks);
    };
}

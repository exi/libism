#pragma once

#include <boost/shared_ptr.hpp>
#include <Eigen/Geometry>
#include "Tracks.hpp"
#include "Object.hpp"
#include "Heuristic.hpp"

namespace ISM {
    class DirectionOrientationRelationHeuristic: public Heuristic {
        public:
            DirectionOrientationRelationHeuristic(const TracksPtr& tracks);
        private:
            Eigen::Vector3d getDirectionVector(const ObjectPtr& first, const ObjectPtr& second);
    };
}

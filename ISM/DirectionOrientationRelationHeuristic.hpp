#pragma once

#include <boost/shared_ptr.hpp>
#include <Eigen/Geometry>
#include "Tracks.hpp"
#include "Object.hpp"
#include "Heuristic.hpp"

#ifndef DORH_STATIC_BREAK_RATIO
#define DORH_STATIC_BREAK_RATIO 0.01
#endif
#ifndef DORH_TOGETHER_RATIO
#define DORH_TOGETHER_RATIO 0.90
#endif
#ifndef DORH_MAX_ANGLE_DEVIATION
#define DORH_MAX_ANGLE_DEVIATION 45
#endif

namespace ISM {
    class DirectionOrientationRelationHeuristic: public Heuristic {
        public:
            DirectionOrientationRelationHeuristic(const TracksPtr& tracks);
        private:
            Eigen::Vector3d getDirectionVector(const ObjectPtr& first, const ObjectPtr& second);
    };
}

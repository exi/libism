#pragma once

#include <boost/shared_ptr.hpp>
#include <Eigen/Geometry>
#include "Tracks.hpp"
#include "Object.hpp"
#include "Heuristic.hpp"

#ifndef DRH_STATIC_BREAK_RATIO
#define DRH_STATIC_BREAK_RATIO 0.01
#endif
#ifndef DRH_TOGETHER_RATIO
#define DRH_TOGETHER_RATIO 0.90
#endif
#ifndef DRH_MAX_ANGLE_DEVIATION
#define DRH_MAX_ANGLE_DEVIATION 45
#endif

namespace ISM {
    class DirectionRelationHeuristic: public Heuristic {
        public:
            DirectionRelationHeuristic(const TracksPtr& tracks);
        private:
            Eigen::Vector3d getDirectionVector(const ObjectPtr& first, const ObjectPtr& second);
    };
}

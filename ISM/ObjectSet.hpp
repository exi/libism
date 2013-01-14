#pragma once

#include <vector>
#include <iostream>
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <limits>
#include <algorithm>
#include "Object.hpp"
#include "Pose.hpp"

namespace ISM {
    class ObjectSet {
        public:
            std::vector<ObjectPtr> objects;

            ~ObjectSet() {
                std::cout<<"destroy object set"<<std::endl;
            }

            void insert(ObjectPtr o) {
                objects.push_back(o);
                std::cout<<"insert "<<(*o)<<std::endl;
            }

            std::vector<ObjectPtr> getObjects() { return this->objects; };

            Point getReferencePoint() {
                double maxX = std::numeric_limits<double>::min();
                double minX = std::numeric_limits<double>::max();
                double maxY = std::numeric_limits<double>::min();
                double minY = std::numeric_limits<double>::max();
                double maxZ = std::numeric_limits<double>::min();
                double minZ = std::numeric_limits<double>::max();
                BOOST_FOREACH(ObjectPtr o, this->objects) {
                    PointPtr p = o->getPose()->point;
                    maxX = std::max(p->x, maxX);
                    minX = std::min(p->x, minX);
                    maxY = std::max(p->y, maxY);
                    minY = std::min(p->y, minY);
                    maxZ = std::max(p->z, maxZ);
                    minZ = std::min(p->z, minZ);
                }

                return Point((minX + maxX) / 2.0, (minY + maxY) / 2.0, (minZ + maxZ) / 2.0);
            };
    };
    typedef boost::shared_ptr<ObjectSet> ObjectSetPtr;
}

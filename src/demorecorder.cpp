#include <ISM/Object.hpp>
#include <ISM/Recorder.hpp>
#include <iostream>

#include <boost/math/constants/constants.hpp>

using namespace ISM;

int main (int argc, char** argv) {
    double directioncos = cos(0.0 * (boost::math::constants::pi<double>() / 180.0));
    double directionsin = sin(0.0 * (boost::math::constants::pi<double>() / 180.0));

    Recorder r;

    ObjectSetPtr set(new ObjectSet());

    ObjectPtr o(new Object("testobj", new Pose(new Point(0.0, 0.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "1"));
    set->insert(o);

    o = ObjectPtr(new Object("testobj", new Pose(new Point(10.0, 10.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "2"));
    set->insert(o);

    o = ObjectPtr(new Object("testobj", new Pose(new Point(10.0, 0.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "3"));
    set->insert(o);

    r.insert(set, "demoSet");

    set = ObjectSetPtr(new ObjectSet());

    o = ObjectPtr(new Object("testobj", new Pose(new Point(2.5, 0.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "1"));
    set->insert(o);

    o = ObjectPtr(new Object("testobj", new Pose(new Point(12.5, 10.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "2"));
    set->insert(o);

    o = ObjectPtr(new Object("testobj", new Pose(new Point(12.5, 0.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "3"));
    set->insert(o);

    r.insert(set, "demoSet");
    return 0;
}

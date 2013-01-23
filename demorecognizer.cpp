#include <ISM/Recognizer.hpp>
#include <ISM/ObjectSet.hpp>
#include <string>
#include <boost/math/constants/constants.hpp>

using namespace ISM;
int main (int argc, char** argv) {
    double directioncos = cos(-45.0 * (boost::math::constants::pi<double>() / 180.0));
    double directionsin = sin(-45.0 * (boost::math::constants::pi<double>() / 180.0));

    Recognizer r;

    ObjectSetPtr set(new ObjectSet());

    ObjectPtr o(new Object("testobj", new Pose(new Point(0.0, 0.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos))));
    set->insert(o);

    o = ObjectPtr(new Object("testobj2", new Pose(new Point(-10.0, 10.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos))));
    set->insert(o);

    o = ObjectPtr(new Object("testobj3", new Pose(new Point(0.0, 10.0, 0.0), new Quaternion(directionsin, 0.0, 0.0, directioncos))));
    set->insert(o);

    r.recognizePattern(set);

    return 0;
}

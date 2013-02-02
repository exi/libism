#include <ISM/Recognizer.hpp>
#include <ISM/ObjectSet.hpp>
#include <string>
#include <boost/math/constants/constants.hpp>
#include <ISM/MathHelper.hpp>

using namespace ISM;
int main (int argc, char** argv) {
    typedef MathHelper MH;

    double directioncos = cos(-45.0 * (boost::math::constants::pi<double>() / 180.0));
    double directionsin = sin(-45.0 * (boost::math::constants::pi<double>() / 180.0));

    Recognizer r(5.0);

    ObjectSetPtr set(new ObjectSet());
    QuaternionPtr quat = QuaternionPtr(new Quaternion(directionsin, 0.0, 0.0, directioncos));

    ObjectPtr o(new Object("testobj", new Pose(PointPtr(new Point(0.5, 0.5, 0.0)), quat)));
    set->insert(o);

    o = ObjectPtr(new Object("testobj2", new Pose(PointPtr(new Point(-8.5, 9.6, 0.0)), quat)));
    set->insert(o);

    o = ObjectPtr(new Object("testobj3", new Pose(PointPtr(new Point(0.0, 10.7, 0.0)), quat)));
    set->insert(o);

    std::cout<<"results:"<<std::endl;
    for (auto& result : r.recognizePattern(set)) {
        std::cout<<result<<std::endl;
        for (auto& item : result->idealPoints) {
            std::cout<<item<<std::endl;
        }
    }

    return 0;
}

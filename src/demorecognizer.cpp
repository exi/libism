#include <ISM/Recognizer.hpp>
#include <ISM/ObjectSet.hpp>
#include <string>
#include <boost/math/constants/constants.hpp>
#include <ISM/MathHelper.hpp>

using namespace ISM;
int main () {
    typedef MathHelper MH;
    double directioncos = cos(0.0 * (boost::math::constants::pi<double>() / 180.0));
    double directionsin = sin(0.0 * (boost::math::constants::pi<double>() / 180.0));
    Recognizer r(0.2);

    ObjectSetPtr set(new ObjectSet());
    ObjectPtr o(new Object("testobj", new Pose(new Point(0.1, 0.1, 0.1), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "1"));
    set->insert(o);

    o = ObjectPtr(new Object("testobj", new Pose(new Point(10.1, 10.1, 0.1), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "2"));
    set->insert(o);

    o = ObjectPtr(new Object("testobj", new Pose(new Point(10.1, 0.1, 0.1), new Quaternion(directionsin, 0.0, 0.0, directioncos)), "3"));
    set->insert(o);

    std::cout<<"results:"<<std::endl;
    for (auto& result : r.recognizePattern(set)) {
        std::cout<<result<<std::endl;
        std::cout<<"ideal:"<<std::endl;
        for (auto& item : result->idealPoints) {
            std::cout<<item<<std::endl;
        }
    }

    return 0;
}

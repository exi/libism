#include <ISM/Recognizer.hpp>
#include <ISM/ObjectSet.hpp>
#include <string>

using namespace ISM;
int main (int argc, char** argv) {
    Recognizer r;

    ObjectSetPtr set(new ObjectSet());
    ObjectPtr o(new Object("testobj", new Pose(new Point(0.0, 0.0, 0.0), new Quaternion(0.0, 1.0, 0.0, 0.0))));
    set->insert(o);
    o = ObjectPtr(new Object("testobj2", new Pose(new Point(10.0, 10.0, 0.0), new Quaternion(0.0, 1.0, 0.0, 0.0))));
    set->insert(o);
    o = ObjectPtr(new Object("testobj3", new Pose(new Point(10.0, 0.0, 0.0), new Quaternion(0.0, 1.0, 0.0, 0.0))));
    set->insert(o);
    r.recognizePattern(set);

    return 0;
}

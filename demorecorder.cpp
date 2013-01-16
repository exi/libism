#include <ISM/Object.hpp>
#include <ISM/Recorder.hpp>
#include <iostream>

using namespace ISM;
int main (int argc, char** argv) {
    Recorder r;
    ObjectSetPtr set(new ObjectSet());
    ObjectPtr o(new Object());
    o->setPose(PosePtr(new Pose(new Point(1.0, 2.0, 3.0), new Quaternion(1.0, 2.0, 3.0, 4.0))));
    o->setType("testobj");
    set->insert(o);
    o = ObjectPtr(new Object());
    o->setPose(PosePtr(new Pose(new Point(4.0, 8.0, 9.0), new Quaternion(1.0, 0.0, 0.0, 0.0))));
    o->setType("testobj2");
    set->insert(o);
    r.insert(set, "demoSet");
    return 0;
}

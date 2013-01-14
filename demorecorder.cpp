#include <ISM/Object.hpp>
#include <ISM/Recorder.hpp>
#include <iostream>

using namespace ISM;
int main (int argc, char** argv) {
    Recorder r;
    boost::shared_ptr<ObjectSet> set(new ObjectSet());
    boost::shared_ptr<Object> o(new Object());
    o->setPose(PosePtr(new Pose(new Point(1.0, 2.0, 3.0), new Quaternion(1.0, 2.0, 3.0, 4.0))));
    o->setType("testobj");
    set->insert(o);
    o = boost::shared_ptr<Object>(new Object());
    o->setPose(PosePtr(new Pose(new Point(2.0, 3.0, 4.0), new Quaternion(2.0, 3.0, 4.0, 5.0))));
    o->setType("testobj2");
    set->insert(o);
    r.insert(set, "demoSet");
    return 0;
}

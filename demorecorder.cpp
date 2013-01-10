#include <ISM/Object.hpp>
#include <ISM/Recorder.hpp>
#include <iostream>

using namespace ISM;
int main (int argc, char** argv) {
    Recorder r;
    boost::shared_ptr<ObjectSet> set(new ObjectSet("demoscene"));
    boost::shared_ptr<Object> o(new ObservedObject());
    o->setPose(Pose(Point(1.0, 2.0, 3.0), Quaternion(1.0, 2.0, 3.0, 4.0)));
    o->setID("testobj");
    set->insert(o);
    o = boost::shared_ptr<Object>(new ObservedObject());
    o->setPose(Pose(Point(2.0, 3.0, 4.0), Quaternion(2.0, 3.0, 4.0, 5.0)));
    o->setID("testobj2");
    set->insert(o);
    r.insert(set, "demoSet");
    return 0;
}

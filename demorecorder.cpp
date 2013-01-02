#include "ISM/Object.hpp"
#include "ISM/Recorder.hpp"

using namespace ISM;
int main (int argc, char** argv) {
    Recorder r;
    Object* o = new ObservedObject();
    o->setPose(Pose(Point(1.0, 2.0, 3.0), Quaternion(1.0, 2.0, 3.0, 4.0)));
    o->setID("testobj");
    r.insert(o);
    return 0;
}

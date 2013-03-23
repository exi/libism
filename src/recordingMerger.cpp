#include <ISM/TableHelper.hpp>
#include <string>
#include <algorithm>

using namespace ISM;
using namespace std;

int main (int argc, char** argv) {
    if (argc == 3) {
        cout<<"merging "<<argv[1]<<" into "<<argv[2]<<endl;
        cout<<"Type yes to continue"<<endl;
        string yes;
        cin>>yes;
        transform(yes.begin(), yes.end(), yes.begin(), ::tolower);
        if (yes != "yes") {
            cout<<"aborting"<<endl;
            return 1;
        }

        TableHelper t(argv[1]);
        TableHelper t2(argv[2]);

        auto patternNames = t.getRecordedPatternNames();
        cout<<"found "<<patternNames.size()<<" patterns"<<endl;
        for (auto& patternName : patternNames) {
            cout<<"merge pattern "<<patternName<<endl;
            auto pattern = t.getRecordedPattern(patternName);
            for (auto& set : pattern->objectSets) {
                t2.insertRecordedObjectSet(set, patternName);
                cout<<".";
                cout.flush();
            }
            cout<<"done"<<endl;
        }
    } else {
        cout<<"Invalid parameter count"<<endl;
        cout<<"Usage: ./recordingMerger <sourcefile> <targetfile>"<<endl;
    }
}

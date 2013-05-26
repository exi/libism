#include <ISM/Recognizer.hpp>
#include <ISM/TableHelper.hpp>
#include <ISM/JsonStream.hpp>
#include <string>
#include <iostream>

using namespace ISM;
using namespace std;
int main (int argc, char** argv) {
    if (argc == 3) {
        Recognizer r(0.05);
        TableHelper t("record.sqlite");
        string patternName(argv[1]);
        long int setId = strtol(argv[2], NULL, 10);
        RecordedPatternPtr p = t.getRecordedPattern(patternName);
        ObjectSetPtr toRecognize = p->objectSets[setId];
        RecognitionResultPtr res = r.recognizePattern(toRecognize)[0];
        cerr<<"{ \"referencePose\": "<<json(res->referencePose)<<",";
        cerr<<"\"objects\": [";
        bool first = true;
        for (auto& maptype : (*res->votedPoints)) {
            if (first) {
                first = false;
            } else {
                cerr<<", ";
            }
            cerr<<"{"
                <<"\"object\": "<<json(maptype.first)<<", "
                <<"\"votes\": [";
            {
                bool first = true;
                for (auto& point : maptype.second) {
                    if (first) {
                        first = false;
                    } else {
                        cerr<<", ";
                    }
                    cerr<<json(point);
                }
            }
            cerr<<"]}";
        }
        cerr<<"]}";
    };
}

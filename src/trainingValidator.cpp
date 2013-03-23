#include <ISM/TableHelper.hpp>
#include <ISM/RecordedPattern.hpp>
#include <ISM/Recognizer.hpp>
#include <string>

using namespace ISM;
using namespace std;

void validatePattern(RecordedPatternPtr pattern, RecognizerPtr recognizer) {
    cout<<"validate pattern "<<pattern->name<<endl;
    int idx = 0;
    double confidenceSum = 0;
    for (auto& set : pattern->objectSets) {
        auto results = recognizer->recognizePattern(set);
        bool found = false;
        for (auto& result : results) {
            if (result->patternName == pattern->name) {
                cout<<".";
                cout.flush();
                confidenceSum += result->confidence;
                found = true;
                break;
            }
        }
        idx++;
    }
    cout<<endl;

    double meanConfidence = confidenceSum / (double)idx;
    cout<<(meanConfidence >= 0.8 ? "SUCCESS" : "FAILURE")<<" mean confidence for pattern '"<<pattern->name<<"' is "<<meanConfidence<<endl;
}

int main (int argc, char** argv) {
    double sensitivity = 0.0001;
    if (argc < 2) {
        TableHelper t;
        RecognizerPtr r(new Recognizer(sensitivity));
        std::cout<<"validating all patterns"<<std::endl;
        auto patternNames = t.getRecordedPatternNames();
        for (auto& name : patternNames) {
            auto pattern = t.getRecordedPattern(name);
            validatePattern(pattern, r);
        }
    } else if (argc == 2) {
        TableHelper t;
        RecognizerPtr r(new Recognizer(sensitivity));
        std::string patternName(argv[1]);
        std::cout<<"validating pattern "<<patternName<<std::endl;
        auto pattern = t.getRecordedPattern(patternName);
        validatePattern(pattern, r);
    } else if (argc == 3) {
        TableHelper t(argv[1]);
        RecognizerPtr r(new Recognizer(argv[1], sensitivity));
        std::string patternName(argv[2]);
        std::cout<<"validating pattern "<<patternName<<" from file "<<argv[1]<<std::endl;
        auto pattern = t.getRecordedPattern(patternName);
        validatePattern(pattern, r);
    }
}

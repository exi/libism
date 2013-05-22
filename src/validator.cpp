#include <ISM/TableHelper.hpp>
#include <ISM/RecordedPattern.hpp>
#include <ISM/Recognizer.hpp>
#include <boost/program_options.hpp>
#include <string>
#include <set>
#include <utility>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;

bool detectGeneric = false;

void validatePattern(RecordedPatternPtr pattern, RecognizerPtr recognizer) {
    cout<<"validate pattern "<<pattern->name<<endl;
    int idx = 0;
    int objectCount = 0;
    int identifySum = 0;
    double confidenceSum = 0;
    double thresholdConfidence = 0.8;
    for (auto& os : pattern->objectSets) {

        set<pair<string, string> > mappedTypes;

        if (detectGeneric) {
            for (auto& obj : os->objects) {
                mappedTypes.insert(make_pair(obj->type, obj->observedId));
                objectCount++;
                obj->type = "";
                obj->observedId = "";
            }
        }

        auto results = recognizer->recognizePattern(os);
        for (auto& result : results) {
            if (result->patternName == pattern->name) {
                cout<<(result->confidence > thresholdConfidence ? "." : ",");
                cout.flush();
                confidenceSum += result->confidence;
                if (result->confidence > thresholdConfidence) {
                    for (auto& obj : result->recognizedSet->objects) {
                        auto match = mappedTypes.find(make_pair(obj->type, obj->observedId));
                        if (match != mappedTypes.end()) {
                            mappedTypes.erase(match);
                            identifySum++;
                        }
                    }
                }
                cout<<result<<endl;
            }
        }
        idx++;
    }
    cout<<endl;

    double meanConfidence = confidenceSum / (double)idx;
    cout<<(meanConfidence >= thresholdConfidence ? "SUCCESS" : "FAILURE")<<" mean confidence for pattern '"<<pattern->name<<"' is "<<meanConfidence<<endl;
    if (detectGeneric) {
        double identifyRate = (double)identifySum / (double)objectCount;
        cout<<"Identification rate: "<<identifyRate<<endl;
    }
}

int main (int argc, char** argv) {
    double sensitivity = 0.0001;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("database-file,r", po::value<string>()->default_value("record.sqlite"), "database file to use")
        ("generic-mode,g", po::bool_switch(&detectGeneric), "test object type inference by removing object type and id from recognition input")
        ("sensitivity,s", po::value<double>(&sensitivity)->default_value(0.0001), "recognizer sensitivity")
        ("pattern-name,p", po::value<vector<string> >(), "patters to validate instead of all")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    TableHelper t(vm["database-file"].as<string>());
    RecognizerPtr r(new Recognizer(vm["database-file"].as<string>(), sensitivity));
    if (vm.count("pattern-name")) {
        auto patternNames = vm["pattern-name"].as<vector<string> >();
        for (auto& name : patternNames) {
            auto pattern = t.getRecordedPattern(name);
            if (!pattern) {
                cout<<"Pattern "<<name<<" not found!";
                continue;
            }
            validatePattern(pattern, r);
        }
    } else {
        auto patternNames = t.getRecordedPatternNames();
        for (auto& name : patternNames) {
            auto pattern = t.getRecordedPattern(name);
            if (!pattern) {
                cout<<"Pattern "<<name<<" not found!";
                continue;
            }
            validatePattern(pattern, r);
        }
    }
}

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
    for (auto& set : pattern->objectSets) {

        std::set<std::pair<string, string> > mappedTypes;

        if (detectGeneric) {
            for (auto& obj : set->objects) {
                mappedTypes.insert(make_pair(obj->type, obj->observedId));
                objectCount++;
                obj->type = "";
                obj->observedId = "";
            }
        }

        auto results = recognizer->recognizePattern(set);
        for (auto& result : results) {
            if (result->patternName == pattern->name) {
                cout<<".";
                cout.flush();
                confidenceSum += result->confidence;

                for (auto& obj : result->recognizedSet->objects) {
                    auto match = mappedTypes.find(make_pair(obj->type, obj->observedId));
                    if (match != mappedTypes.end()) {
                        mappedTypes.erase(match);
                        identifySum++;
                    }
                }

                break;
            }
        }
        idx++;
    }
    cout<<endl;

    double meanConfidence = confidenceSum / (double)idx;
    cout<<(meanConfidence >= 0.8 ? "SUCCESS" : "FAILURE")<<" mean confidence for pattern '"<<pattern->name<<"' is "<<meanConfidence<<endl;
    if (detectGeneric) {
        double identifyRate = (double)identifySum / (double)objectCount;
        cout<<"Identification rate: "<<identifyRate<<endl;
    }
}

int main (int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("database-file,r", po::value<string>()->default_value("record.sqlite"), "database file to use")
        ("generic-mode,g", po::bool_switch(&detectGeneric), "test object type inference by removing object type and id from recognition input")
        ("pattern-name,p", po::value<vector<string> >(), "patters to validate instead of all")
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    double sensitivity = 0.0001;
    TableHelper t(vm["database-file"].as<string>());
    RecognizerPtr r(new Recognizer(vm["database-file"].as<string>(), sensitivity));
    if (vm.count("pattern-name")) {
        auto patternNames = vm["pattern-name"].as<vector<string> >();
        for (auto& name : patternNames) {
            auto pattern = t.getRecordedPattern(name);
            validatePattern(pattern, r);
        }
    } else {
        auto patternNames = t.getRecordedPatternNames();
        for (auto& name : patternNames) {
            auto pattern = t.getRecordedPattern(name);
            validatePattern(pattern, r);
        }
    }
}

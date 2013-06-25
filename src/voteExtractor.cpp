#include <ISM/Recognizer.hpp>
#include <ISM/TableHelper.hpp>
#include <ISM/JsonStream.hpp>
#include <string>
#include <iostream>
#include <boost/program_options.hpp>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;

int main(int argc, char** argv) {
    bool detectGeneric = false;
    double sensitivity;
    size_t setId;

    po::options_description desc("Allowed options");
    desc.add_options()("help,h", "produce help message")
            ("database-file,d", po::value<string>()->default_value("record.sqlite"), "database file to use")
            ("source-database-file,S", po::value<string>()->default_value("record.sqlite"), "data source file to use")
            ("generic-mode,g", po::bool_switch(&detectGeneric),
                    "test object type inference by removing object type and id from recognition input")
            ("sensitivity,s", po::value<double>(&sensitivity)->default_value(0.005), "recognizer sensitivity")
            ("pattern-name,p", po::value<string>(), "pattern to use (default: first found)")
            ("set-number,n", po::value<size_t>(&setId)->default_value(0), "set number to use");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    Recognizer r(vm["database-file"].as<string>(), sensitivity);
    TableHelper t(vm["database-file"].as<string>());
    TableHelper ts(vm["source-database-file"].as<string>());
    string patternName;
    if (vm.count("pattern-name")) {
        patternName = vm["pattern-name"].as<string>();
    } else {
        vector<string> patterns = ts.getRecordedPatternNames();
        if (patterns.size() == 0) {
            cout << "no patterns found" << endl;
            return 1;
        }
        patternName = patterns[0];
    }
    RecordedPatternPtr p = ts.getRecordedPattern(patternName);
    if (setId >= p->objectSets.size()) {
        cout << "set has only " << p->objectSets.size() << " elements" << endl;
        return 1;
    }
    ObjectSetPtr toRecognize = p->objectSets[setId];
    RecognitionResultPtr res = r.recognizePattern(toRecognize)[0];
    cerr << "{ \"referencePose\": " << json(res->referencePose) << ",";
    cerr << "\"objects\": [";
    bool first = true;
    for (auto& maptype : (*res->votedPoints)) {
        if (first) {
            first = false;
        } else {
            cerr << ", ";
        }
        cerr << "{" << "\"object\": " << json(maptype.first) << ", " << "\"votes\": [";
        {
            bool first = true;
            for (auto& pointAndConf : maptype.second) {
                if (first) {
                    first = false;
                } else {
                    cerr << ", ";
                }
                cerr << "{\"point\":" << json(pointAndConf.first) << ",\"confidence\":" << pointAndConf.second<<"}";
            }
        }
        cerr << "]}";
    }
    cerr << "]}";
}

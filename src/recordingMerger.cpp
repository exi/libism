#include <ISM/TableHelper.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <boost/program_options.hpp>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;

int main (int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("target-file,t", po::value<string>()->default_value("record.sqlite"), "target file")
        ("source-file,s", po::value<vector<string> >(), "source file(s)")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("target-file") || !vm.count("source-file")) {
        cout << desc << "\n";
        return 1;
    }

    bool first = true;
    auto sourceFiles = vm["source-file"].as<vector<string> >();
    auto targetFile = vm["target-file"].as<string>();

    cout<<"merging ";
    for (auto& source : sourceFiles) {
        if (!first) {
            cout<<",";
        } else {
            first = false;
        }
        cout<<source;
    }
    cout<<" into "<<targetFile<<endl;
    cout<<"Type yes to continue"<<endl;
    string yes;
    cin>>yes;
    transform(yes.begin(), yes.end(), yes.begin(), ::tolower);
    if (yes != "yes") {
        cout<<"aborting"<<endl;
        return 1;
    }

    TableHelper target(targetFile);
    for (auto& sourceFile : sourceFiles) {
        TableHelper source(sourceFile);
        cout<<"merging "<<sourceFile<<endl;

        auto patternNames = source.getRecordedPatternNames();
        cout<<"found "<<patternNames.size()<<" patterns"<<endl;
        for (auto& patternName : patternNames) {
            cout<<"merge pattern "<<patternName<<endl;
            auto pattern = source.getRecordedPattern(patternName);
            for (auto& set : pattern->objectSets) {
                target.insertRecordedObjectSet(set, patternName);
                cout<<".";
                cout.flush();
            }
            cout<<"done"<<endl;
        }
    }
}

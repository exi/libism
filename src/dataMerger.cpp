#include <ISM/TableHelper.hpp>
#include <string>
#include <vector>
#include <algorithm>
#include <set>
#include <boost/program_options.hpp>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;

int main (int argc, char** argv) {
    bool mergeRecordings = false;
    bool mergeModels = false;

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("target-file,t", po::value<string>()->default_value("record.sqlite"), "target file")
    ;

    po::options_description required("Required");
    required.add_options()
        ("source-file,s", po::value<vector<string> >(), "source file(s)")
    ;

    po::options_description atLeastOne("At least one of these");
    atLeastOne.add_options()
        ("merge-recording,r", po::bool_switch(&mergeRecordings), "merge recordings")
        ("merge-models,m", po::bool_switch(&mergeModels), "merge models")
    ;

    desc.add(required);
    desc.add(atLeastOne);

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("target-file") || !vm.count("source-file") || (!mergeRecordings && !mergeModels)) {
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
        cout<<sourceFile<<":"<<endl;

        if (mergeRecordings) {
            cout<<"merging recordings"<<endl;
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

        if (mergeModels) {
            cout<<"merging models"<<endl;
            auto objectTypes = source.getObjectTypes();
            cout<<"found "<<objectTypes.size()<<" object types"<<endl;
            auto typeToVoteMap = source.getVoteSpecifiersForObjectTypes(objectTypes);
            set<string> patternNames;
            for (auto& typeToVoteVecPair : typeToVoteMap) {
                auto type = typeToVoteVecPair.first;
                auto voteVec = typeToVoteVecPair.second;
                cout<<"found "<<voteVec.size()<<" votes for object type "<<type<<endl;
                for (auto& vote : voteVec) {
                    patternNames.insert(vote->patternName);
                    target.insertModelVoteSpecifier(vote);
                    cout<<".";
                    cout.flush();
                }
                cout<<"done"<<endl;
            }
            auto patternMap = source.getPatternDefinitionsByName(patternNames);
            cout<<"found "<<patternMap.size()<<" patterns"<<endl;
            for (auto& patternNameToPattern : patternMap) {
                auto pattern = patternNameToPattern.second;
                target.upsertModelPattern(pattern->name, pattern->expectedObjectCount, pattern->referencePointSpread);
                cout<<".";
                cout.flush();
            }
            cout<<"done"<<endl;
        }
    }
}

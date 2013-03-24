#include <ISM/TableHelper.hpp>
#include <string>
#include <algorithm>
#include <boost/program_options.hpp>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;

int main (int argc, char** argv) {
    po::positional_options_description p;
    p.add("database-file", -1);

    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("database-file,d", po::value<vector<string> >(), "database file")
    ;

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    po::notify(vm);

    if (vm.count("help") || !vm.count("database-file")) {
        cout << desc << "\n";
        return 1;
    }

    auto databaseFiles = vm["database-file"].as<vector<string> >();

    cout<<"removing models from ";

    bool first = true;
    for (auto& database : databaseFiles) {
        if (!first) {
            cout<<",";
        } else {
            first = false;
        }
        cout<<database;
    }
    cout<<" ?"<<endl;
    cout<<"Type yes to continue"<<endl;
    string yes;
    cin>>yes;
    transform(yes.begin(), yes.end(), yes.begin(), ::tolower);
    if (yes != "yes") {
        cout<<"aborting"<<endl;
        return 1;
    }


    for (auto& databaseFile : databaseFiles) {
        TableHelper database(databaseFile);
        cout<<"deleting model from "<<databaseFile<<endl;
        database.dropModelTables();
    }
}

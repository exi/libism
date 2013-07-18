#include <ISM/Trainer.hpp>
#include <boost/program_options.hpp>
#include <string>

using namespace ISM;
using namespace std;
namespace po = boost::program_options;

int main (int argc, char** argv) {
    po::options_description desc("Allowed options");
    desc.add_options()
        ("help,h", "produce help message")
        ("database-file,d", po::value<string>()->default_value("record.sqlite"), "database file to use")
        ("pattern-name,p", po::value<vector<string> >(), "patters to train instead of all")
        ("skip-count,s", po::value<int>(), "skip number of training sets after every trained set")
    ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc << "\n";
        return 1;
    }

    string filename = vm["database-file"].as<string>();

    Trainer t(filename);
    if (vm.count("skip-count")) {
        t.setSkipsPerCycle(vm["skip-count"].as<int>());
    }
    std::string patternName(argv[2]);
    if (vm.count("pattern-name")) {
        auto names = vm["pattern-name"].as<vector<string> >();
        for (auto& patternName : names) {
            std::cout<<"training pattern "<<patternName<<" from file "<<filename<<std::endl;
            t.trainPattern(patternName);
        }
    } else {
        t.trainPattern();
    }
}

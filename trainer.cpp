#include <ISM/Trainer.hpp>
#include <string>

using namespace ISM;
int main (int argc, char** argv) {
    if (argc <= 2) {
        Trainer t;
        std::cout<<"training all patterns"<<std::endl;
        t.trainPattern();
    } else if (argc == 2) {
        Trainer t;
        std::string patternName(argv[1]);
        std::cout<<"training pattern "<<patternName<<std::endl;
        t.trainPattern(patternName);
    } else if (argc == 3) {
        Trainer t(argv[1]);
        std::string patternName(argv[2]);
        std::cout<<"training pattern "<<patternName<<" from file "<<argv[1]<<std::endl;
        t.trainPattern(patternName);
    }
}

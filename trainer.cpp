#include <ISM/Trainer.hpp>
#include <string>

using namespace ISM;
int main (int argc, char** argv) {
    if (argc != 2) return -1;

    Trainer t;
    std::string patternName(argv[1]);
    std::cout<<"training pattern "<<patternName<<std::endl;
    t.trainPattern(patternName);
}

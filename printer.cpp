#include <ISM/Trainer.hpp>
#include <string>
#include <iostream>

using namespace ISM;
int main (int argc, char** argv) {
    if (argc == 2) {
        Trainer t;
        std::string patternName(argv[1]);
        std::cerr<<t.getJsonRepresentation(patternName)<<std::endl;
    };
}

#include "Trainer.hpp"
#include <vector>

namespace ISM {
    Trainer::Trainer(std::string dbfilename): tableHelper(dbfilename) {
    }

    Trainer::~Trainer() {
        std::cout<<"recorder destroyed"<<std::endl;
    }

    Trainer::trainPattern(std::string patternName) {

    }
}

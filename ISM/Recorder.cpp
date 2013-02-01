#include "Recorder.hpp"
#include <vector>

namespace ISM {
    Recorder::Recorder(const std::string& dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    Recorder::~Recorder() {
        std::cout<<"recorder destroyed"<<std::endl;
    }

    void Recorder::insert(const ObjectSetPtr& set, const std::string& patternName) {
        std::cout<<"inserting set for pattern "<<patternName<<". With Objects: "<<set<<std::endl;
        this->tableHelper->insertRecordedObjectSet(set, patternName);
    }
}

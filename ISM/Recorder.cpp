#include "Recorder.hpp"
#include <vector>

namespace ISM {
    Recorder::Recorder(std::string dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    Recorder::~Recorder() {
        std::cout<<"recorder destroyed"<<std::endl;
    }

    void Recorder::insert(boost::shared_ptr<ObjectSet> set, std::string patternName) {
        std::cout<<"inserting set for pattern "<<patternName<<". With Objects:"<<std::endl;
        this->tableHelper->insertRecordedObjectSet(set, patternName);
    }
}

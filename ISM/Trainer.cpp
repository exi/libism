#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>

#include "Trainer.hpp"
#include "RecordedPattern.hpp"

namespace ISM {
    Trainer::Trainer(std::string dbfilename): tableHelper(dbfilename) {
    }

    Trainer::~Trainer() {
        std::cout<<"recorder destroyed"<<std::endl;
    }

    void Trainer::trainPattern(std::string patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper.getRecordedPattern(patternName);
        if (!r) {
            std::cout<<"no pattern records found"<<std::endl;
        } else {
            std::cout<<"pattern records found:"<<std::endl;
            std::vector<boost::shared_ptr<ObjectSet> > sets = r->getObjectSets();
            for (size_t i = 0; i < sets.size(); i++) {
                std::cout<<"object set "<<i<<std::endl;
                std::vector<boost::shared_ptr<Object> > obs = sets[i]->getObjects();
                for (size_t e = 0; e < obs.size(); e++) {
                    std::cout<<"    object "<<e<<": "<<obs[e]->toString()<<std::endl;
                }
            }

            this->learn(r, patternName);
        }
    }

    void Trainer::learn(boost::shared_ptr<RecordedPattern> pattern, std::string patternName) {

    }
}

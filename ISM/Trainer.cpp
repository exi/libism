#include <boost/shared_ptr.hpp>
#include <vector>
#include <iostream>
#include <set>

#include "Trainer.hpp"
#include "RecordedPattern.hpp"

namespace ISM {
    Trainer::Trainer(std::string dbfilename): tableHelper(dbfilename) {
    }

    Trainer::~Trainer() {
        std::cout<<"recorder destroyed"<<std::endl;
    }

    void Trainer::trainPattern(std::string patternName) {
        this->patternName = patternName;

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
                    std::cout<<"    object "<<e<<": "<<(*obs[e])<<std::endl;
                }
            }

            this->recordedPattern = r;
            this->learn();
        }
    }

    void Trainer::learn() {
        this->calculateAbsoluteReferencePoint();
    }

    void Trainer::calculateAbsoluteReferencePoint() {
        double x = 0, y = 0, z = 0;
        int count = 0;
        std::vector<ObjectSetPtr> sets = this->recordedPattern->getObjectSets();

        BOOST_FOREACH(ObjectSetPtr os, sets) {
            Point p = os->getReferencePoint();
            x += p.x;
            y += p.y;
            z += p.z;
            count++;
        }

        this->absoluteReferencePoint.x = x / (double)count;
        this->absoluteReferencePoint.y = y / (double)count;
        this->absoluteReferencePoint.z = z / (double)count;
        std::cout<<"absolute Reference Point: "<<this->absoluteReferencePoint<<std::endl;
    }
}

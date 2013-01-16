#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <iostream>
#include <set>

#include "Trainer.hpp"
#include "RecordedPattern.hpp"
#include "MathHelper.hpp"
#include "VoteSpecifier.hpp"

namespace ISM {
    Trainer::Trainer(std::string dbfilename): tableHelper(dbfilename) { }

    void Trainer::trainPattern(std::string patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper.getRecordedPattern(patternName);
        if (!r) {
            std::cout<<"no pattern records found."<<std::endl;
        } else {
            std::cout<<"pattern records found."<<std::endl;
            this->recordedPattern = r;
            this->learn();
        }
    }

    void Trainer::learn() {
        PointPtr referencePoint = this->recordedPattern->getAbsoluteReferencePoint();
        std::cout<<"Reference Point: "<<referencePoint<<std::endl;

        std::vector<ObjectSetPtr> sets = this->recordedPattern->getObjectSets();
        BOOST_FOREACH(ObjectSetPtr os, sets) {
            std::vector<ObjectPtr> objects = os->getObjects();
            BOOST_FOREACH(ObjectPtr o, objects) {
                VoteSpecifierPtr vote = MathHelper::getVoteSpecifierToPoint(o, referencePoint);
                vote->setPatternName(this->recordedPattern->getName());
                std::cout<<"projecting "<<vote<<std::endl;
                std::cout<<"projected point: "<<MathHelper::projectVoteOntoPoint(vote)<<std::endl;
            }
        }
    }
}

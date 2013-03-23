#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <iostream>
#include <set>

#include "Trainer.hpp"
#include "RecordedPattern.hpp"
#include "MathHelper.hpp"
#include "VoteSpecifier.hpp"
#include "Pose.hpp"
#include "JsonStream.hpp"

namespace ISM {
    Trainer::Trainer(std::string dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    void Trainer::trainPattern() {
        std::vector<std::string> patternNames = this->tableHelper->getRecordedPatternNames();
        for (auto& name : patternNames) {
            std::cout<<"training "<<name<<std::endl;
            this->trainPattern(name);
        }
    }

    void Trainer::trainPattern(const std::string& patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
        if (!r) {
            std::cout<<"no pattern records found."<<std::endl;
        } else {
            std::cout<<"pattern records found."<<std::endl;
            this->recordedPattern = r;
            this->learn(false);
        }
    }

    void Trainer::learn(bool generateJson) {
        this->json<<"{";
        PointPtr referencePoint = this->recordedPattern->getAbsoluteReferencePoint();
        if (!generateJson) {
            std::cout<<"Reference Point: "<<referencePoint<<std::endl;
        } else {
            this->json<<"\"name\": \""<<this->recordedPattern->name<<"\", "
                <<"\"referencePoint\": "<<ISM::json(referencePoint)<<", "<<std::endl<<"\"objects\": [";
        }

        std::vector<ObjectSetPtr> sets = this->recordedPattern->objectSets;
        int objectCount = 0;
        bool first = true;
        BOOST_FOREACH(ObjectSetPtr os, sets) {
            std::vector<ObjectPtr> objects = os->objects;
            objectCount += objects.size();
            BOOST_FOREACH(ObjectPtr o, objects) {
                VoteSpecifierPtr vote = MathHelper::getVoteSpecifierToPoint(o->pose, referencePoint);
                vote->patternName = this->recordedPattern->name;
                vote->observedId = o->observedId;
                vote->objectType = o->type;
                if (!generateJson) {
                    std::cout<<o<<std::endl;
                    this->tableHelper->insertModelVoteSpecifier(vote);
                } else {
                    if (first) {
                        first = false;
                    } else {
                        this->json<<", "<<std::endl;
                    }
                    this->json<<ISM::json(o);
                }
            }
        }

        if (!generateJson) {
            this->tableHelper->upsertModelPattern(
                this->recordedPattern->name,
                objectCount / sets.size(),
                this->recordedPattern->minMaxFinder->getMaxSpread()
            );
        } else {
            this->json<<"]"<<std::endl;
            this->json<<"}";
        }
    }

    std::string Trainer::getJsonRepresentation(const std::string& patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
        if (r) {
            this->recordedPattern = r;
            this->learn(true);
            return this->json.str();
        } else {
            return "";
        }
    }
}

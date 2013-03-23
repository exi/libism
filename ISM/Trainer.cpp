#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>
#include <vector>
#include <iostream>
#include <set>
#include <math.h>

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
        std::cout<<"found "<<patternNames.size()<<" patterns"<<std::endl;
        for (auto& name : patternNames) {
            this->trainPattern(name);
        }
    }

    void Trainer::trainPattern(const std::string& patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
        if (!r) {
            std::cout<<"no pattern records found for pattern "<<patternName<<std::endl;
        } else {
            std::cout<<"training "<<patternName<<std::endl;
            this->recordedPattern = r;
            this->learn(false);
        }
    }

    void Trainer::learn(bool generateJson) {
        this->json<<"{";
        std::vector<ObjectSetPtr> sets = this->recordedPattern->objectSets;
        int objectCount = 0;
        bool first = true;
        for (ObjectSetPtr& os : sets) {
            PointPtr referencePoint = os->getReferencePoint();
            std::vector<ObjectPtr> objects = os->objects;
            objectCount += objects.size();
            for (ObjectPtr& o : objects) {
                VoteSpecifierPtr vote = MathHelper::getVoteSpecifierToPoint(o->pose, referencePoint);
                vote->patternName = this->recordedPattern->name;
                vote->observedId = o->observedId;
                vote->objectType = o->type;
                if (!generateJson) {
                    std::cout<<".";
                    std::cout.flush();
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
            std::cout<<"done"<<std::endl;
            this->tableHelper->upsertModelPattern(
                this->recordedPattern->name,
                floor(((float)objectCount / (float)sets.size()) + 0.5),
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
            this->json.str("");
            this->json.clear();
            this->recordedPattern = r;
            this->learn(true);
            return this->json.str();
        } else {
            return "";
        }
    }
}

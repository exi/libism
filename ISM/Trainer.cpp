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

    void Trainer::trainPattern(std::string patternName) {
        boost::shared_ptr<RecordedPattern> r = this->tableHelper->getRecordedPattern(patternName);
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

        std::vector<ObjectSetPtr> sets = this->recordedPattern->objectSets;
        int objectCount = 0;
        BOOST_FOREACH(ObjectSetPtr os, sets) {
            std::vector<ObjectPtr> objects = os->objects;
            objectCount += objects.size();
            BOOST_FOREACH(ObjectPtr o, objects) {
                VoteSpecifierPtr vote = MathHelper::getVoteSpecifierToPoint(o->pose, referencePoint);
                vote->patternName = this->recordedPattern->name;
                vote->observedId = o->observedId;
                vote->objectType = o->type;
                std::cout<<o<<std::endl;
                auto rpoint = MathHelper::applyQuatAndRadiusToPose(o->pose, vote->objectToRefQuat, vote->radius);
                auto rpose = MathHelper::getReferencePose(o->pose, rpoint, vote->refToObjectQuat);
                std::cout<<"rpose:"<<rpose<<std::endl;
                std::cout<<"toRefPoint:"<<MathHelper::applyQuatAndRadiusToPose(
                        PosePtr(new Pose(PointPtr(new Point()), o->pose->quat)), vote->objectToRefQuat, vote->radius)<<std::endl;
                auto bpoint = MathHelper::getOriginPoint(rpose, vote->objectToRefQuat, vote->refToObjectQuat, vote->radius);
                std::cout<<"bpoint:"<<bpoint<<std::endl;
                std::cout<<"distance:"<<MathHelper::getDistanceBetweenPoints(o->pose->point, bpoint)<<std::endl;
                this->tableHelper->insertModelVoteSpecifier(vote);
            }
        }

        this->tableHelper->upsertModelPattern(
            this->recordedPattern->name,
            objectCount / sets.size(),
            this->recordedPattern->minMaxFinder->getMaxSpread()
        ) ;
    }
}

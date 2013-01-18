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
                std::cout<<"processing "<<o<<std::endl;
                std::cout<<"projecting "<<vote<<std::endl;
                std::cout<<"projected point: "<<MathHelper::applyQuatAndRadiusToPose(o->pose, vote->objectToRefQuat, vote->radius)<<std::endl;
                std::cout<<"backprojected pose vector: "<<
                    MathHelper::vectorToPoint(
                        MathHelper::getPoseVectorFromPose(
                            MathHelper::getReferencePose(o->pose, referencePoint, vote->refToObjectQuat)
                        )
                    )<<std::endl;
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

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
        this->skips = 0;
    }

    void Trainer::setSkipsPerCycle(int skips) {
        this->skips = skips;
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
        typedef MathHelper MH;
        this->json<<"{";
        std::vector<ObjectSetPtr> sets = this->recordedPattern->objectSets;
        int toSkip = 0;
        int setCount = 0;
        int objectCount = 0;
        bool first = true;
        std::string refType = "";
        std::string refId = "";
        for (ObjectSetPtr& os : sets) {
            if (toSkip == 0) {
                toSkip = this->skips;
                std::cout<<".";
                std::cout.flush();
            } else {
                std::cout<<"_";
                std::cout.flush();
                toSkip--;
                continue;
            }

            PosePtr referencePose;
            setCount++;
            std::vector<ObjectPtr> objects = os->objects;

            if (objectCount == 0) {
                // pick reference object at random
                ObjectPtr robj = objects[0];
                referencePose = PosePtr(new Pose(*(robj->pose)));
                refType = robj->type;
                refId = robj->observedId;
                //std::cout<<std::endl<<"chosen ref pose: "<<MH::vectorToPoint(MH::getPoseVectorFromQuat(referencePose->quat))<<std::endl;
            } else {
                // try to find the ref object again
                for (auto& o : objects) {
                    if (o->type == refType && o->observedId == refId) {
                        referencePose = PosePtr(new Pose(*(o->pose)));
                        break;
                    }
                }
                if (!referencePose) {
                    referencePose = PosePtr(new Pose(*(objects[0]->pose)));
                    std::cout<<std::endl<<"cannot reidentify refobj"<<std::endl;
                }
            }

            objectCount += objects.size();
            for (ObjectPtr& o : objects) {
                VoteSpecifierPtr vote = MathHelper::getVoteSpecifierToPose(o->pose, referencePose);
                vote->patternName = this->recordedPattern->name;
                vote->observedId = o->observedId;
                vote->objectType = o->type;
                if (!generateJson) {
                    this->tableHelper->insertModelVoteSpecifier(vote);
                    //auto rpoint = MH::applyQuatAndRadiusToPose(o->pose, vote->objectToRefQuat, vote->radius);
                    //auto rpose = MH::getReferencePose(o->pose, rpoint, vote->objectToRefPoseQuat);
                    //auto bpoint = MH::applyQuatAndRadiusToPose(rpose, vote->refToObjectQuat, vote->radius);
                    //std::cout<<"projected pose:"<<MH::vectorToPoint(MH::getPoseVectorFromQuat(rpose->quat))<<std::endl;
                    //std::cout<<"projected pose errors:"<<
                        //(MH::getPoseVectorFromQuat(rpose->quat) - MH::getPoseVectorFromQuat(referencePose->quat)).norm()
                        //<<","<<MH::getDistanceBetweenPoints(referencePose->point, rpose->point)<<std::endl;
                    //std::cout<<"error:"<<MH::getDistanceBetweenPoints(o->pose->point, bpoint)<<std::endl;
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
                floor(((float)objectCount / (float)setCount) + 0.5),
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

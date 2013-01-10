#include "Recorder.hpp"
#include <vector>

namespace ISM {
    Recorder::Recorder(std::string dbfilename): tableHelper(dbfilename) {
        this->tableHelper.createTablesIfNecessary();
        std::cout<<"lastid: "<<this->tableHelper.getLastInsertId("objects")<<std::endl;
    }

    Recorder::~Recorder() {
        std::cout<<"recorder destroyed"<<std::endl;
    }

    void Recorder::insert(boost::shared_ptr<ObjectSet> set, std::string sceneName) {
        std::cout<<"inserting set for scene "<<set->getSceneName()<<". With Objects:"<<std::endl;
        this->tableHelper.insertObjectSet(set, sceneName);
    }

}

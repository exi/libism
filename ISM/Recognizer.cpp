#include "Recognizer.hpp"

#include <map>
#include <set>
#include <vector>
#include <iostream>
#include <boost/foreach.hpp>

namespace ISM {
    Recognizer::Recognizer(std::string dbfilename) {
        this->tableHelper.reset(new TableHelper(dbfilename));
    }

    std::vector<RecognitionResultPtr> Recognizer::recognizePattern(ObjectSetPtr objectSet) {
        std::set<std::string> objectTypes;
        BOOST_FOREACH(ObjectPtr o, objectSet->objects) {
            objectTypes.insert(o->type);
        }

        objectTypeToVoteMap objectDefinitions = this->tableHelper->getVoteSpecifiersForObjectTypes(objectTypes);
        typedef std::pair<std::string, std::vector<VoteSpecifierPtr> > mapType;
        BOOST_FOREACH(mapType item, objectDefinitions) {
            std::cout<<"type: "<<item.first<<std::endl;
            BOOST_FOREACH(VoteSpecifierPtr vote, item.second) {
                std::cout<<vote<<std::endl;
            }
        }

        std::vector<RecognitionResultPtr> ret;
        return ret;
    }
}

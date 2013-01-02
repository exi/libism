#pragma once

#include "ObjectSet.hpp"
#include "Object.hpp"
#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <boost/shared_ptr.hpp>
#include <soci.h>
#include <soci-sqlite3.h>

using namespace soci;
namespace ISM {
    class Recorder {
        public:
            boost::shared_ptr<session> sqlite;

            Recorder(std::string dbfilename = std::string("record.sqlite")) {
                std::cout<<"recorder with filename "<<dbfilename<<std::endl;
                this->sqlite.reset(new session(sqlite3, dbfilename.c_str()));
                this->createTablesIfNecessary();
            };

            ~Recorder() {
                std::cout<<"recorder destroyed"<<std::endl;
            };

            void createTablesIfNecessary() {
                std::set<std::string> tables;

                rowset<row> rs = (*sqlite).prepare<< "select tbl_name from sqlite_master";
                for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
                    row const& row = *it;
                    std::cout<<row.get<std::string>(0)<<std::endl;
                    tables.insert(row.get<std::string>(0));
                }

                if (tables.find(std::string("objects")) == tables.end()) {

                }
            };

            void insert(boost::shared_ptr<ObjectSet> set) {
                std::cout<<"inserting set for scene "<<set->getSceneName()<<". With Objects:"<<std::endl;

                std::vector<boost::shared_ptr<Object> > objects = set->getObjects();
                for (size_t i = 0; i < objects.size(); i++) {
                    boost::shared_ptr<Object> o = objects[i];
                    std::cout<<o->toString()<<std::endl;
                }
            }
    };
}

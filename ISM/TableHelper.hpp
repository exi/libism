#pragma once

#include "soci/src/core/soci.h"
#include "soci/src/backends/sqlite3/soci-sqlite3.h"
#include "Object.hpp"
#include "ObjectSet.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using namespace soci;
namespace ISM {
    class TableHelper {
        boost::shared_ptr<session> sqlite;
        public:
            TableHelper(std::string dbfilename);

            void createTablesIfNecessary();
            void createObjectsTable();
            void createSetsTable();
            void createObject2SetTable();
            void createScenesTable();
            void createTable(std::string tablename, std::string sql);

            int insertObject(boost::shared_ptr<Object> o);
            int insertObjectSet(boost::shared_ptr<ObjectSet> os, std::string sceneName);
            int insertScene(std::string sceneName);

            int ensureSceneName(std::string sceneName);

            int getLastInsertId(std::string tablename);
    };
}

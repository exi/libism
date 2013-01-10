#pragma once

#include "soci/src/core/soci.h"
#include "soci/src/backends/sqlite3/soci-sqlite3.h"
#include "Object.hpp"
#include "ObjectSet.hpp"
#include "RecordedPattern.hpp"
#include <boost/shared_ptr.hpp>
#include <string>

using namespace soci;
namespace ISM {
    class TableHelper {
        boost::shared_ptr<session> sqlite;
        public:
            TableHelper(std::string dbfilename);

            ~TableHelper();

            void createTablesIfNecessary();
            void createObjectsTable();
            void createSetsTable();
            void createObject2SetTable();
            void createPatternsTable();
            void createTable(std::string tablename, std::string sql);

            int insertObject(boost::shared_ptr<Object> o, int setId);
            int insertObjectSet(boost::shared_ptr<ObjectSet> os, std::string patternName);
            int insertPattern(std::string patternName);

            int ensurePatternName(std::string patternName);

            int getPatternId(std::string patternName);
            int getLastInsertId(std::string tablename);
            boost::shared_ptr<RecordedPattern> getRecordedPattern(std::string patternName);
            boost::shared_ptr<ObjectSet> getObjectSet(int setId);
    };
}

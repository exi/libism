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
            /* util */
            TableHelper(std::string dbfilename);

            ~TableHelper();

            void createTablesIfNecessary();
            void createTable(std::string tablename, std::string sql);

            int getLastInsertId(std::string tablename);

            /* record */
            int insertRecordedObject(boost::shared_ptr<Object> o, int setId);
            int insertRecordedObjectSet(boost::shared_ptr<ObjectSet> os, std::string patternName);
            int insertRecordedPattern(std::string patternName);

            int getRecordedPatternId(std::string patternName);
            int ensureRecordedPatternName(std::string patternName);

            boost::shared_ptr<RecordedPattern> getRecordedPattern(std::string patternName);
            boost::shared_ptr<ObjectSet> getRecordedObjectSet(int setId);

            /* model */
            int insertModelPattern(std::string patternName);

            int getModelPatternId(std::string patternName);
            int ensureModelPatternName(std::string patternName);
    };
}

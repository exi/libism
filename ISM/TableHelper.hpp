#pragma once

#include <boost/shared_ptr.hpp>
#include <string>
#include <map>
#include <set>

#include "soci/src/core/soci.h"
#include "soci/src/backends/sqlite3/soci-sqlite3.h"
#include "Object.hpp"
#include "ObjectSet.hpp"
#include "RecordedPattern.hpp"
#include "VoteSpecifier.hpp"

using namespace soci;
namespace ISM {
    typedef std::map<std::string, std::vector<VoteSpecifierPtr> > objectTypeToVoteMap;
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
            int insertModelVoteSpecifier(VoteSpecifierPtr vote);
            int insertModelPattern(std::string patternName);
            int upsertModelPattern(std::string patternName, int expectedObjectCount, double referencePointSpread);
            int insertModelObjectType(std::string objectType);

            int getModelPatternId(std::string patternName);
            int getModelObjectTypeId(std::string objectType);
            int ensureModelPatternName(std::string patternName);
            int ensureModelObjectType(std::string objectType);

            objectTypeToVoteMap getVoteSpecifiersForObjectTypes(std::set<std::string> objectTypes);
    };

    typedef boost::shared_ptr<TableHelper> TableHelperPtr;
}

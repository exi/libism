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
#include "Pattern.hpp"

using namespace soci;
namespace ISM {
    typedef std::map<std::string, std::vector<VoteSpecifierPtr> > ObjectTypeToVoteMap;
    typedef std::map<std::string, PatternPtr> PatternNameToPatternMap;
    class TableHelper {
        boost::shared_ptr<session> sqlite;
        public:
            /* util */
            TableHelper(std::string dbfilename = "record.sqlite");

            ~TableHelper();

            void createTablesIfNecessary() const;
            void createTable(const std::string& tablename, const std::string& sql) const;
            void dropTable(const std::string& tablename) const;

            void dropModelTables() const;

            int getLastInsertId(const std::string& tablename) const;

            /* record */
            int insertRecordedObject(const boost::shared_ptr<Object>& o, int setId) const;
            int insertRecordedObjectSet(const boost::shared_ptr<ObjectSet>& os, const std::string& patternName) const;
            int insertRecordedPattern(const std::string& patternName) const;
            int insertRecordedPattern(const RecordedPatternPtr& pattern) const;

            int getRecordedPatternId(const std::string& patternName) const;
            int ensureRecordedPatternName(const std::string& patternName) const;
            std::vector<std::string> getRecordedPatternNames() const;

            const RecordedPatternPtr getRecordedPattern(const std::string& patternName) const;
            const ObjectSetPtr getRecordedObjectSet(int setId) const;

            /* model */
            int insertModelVoteSpecifier(const VoteSpecifierPtr& vote) const;
            int insertModelPattern(const std::string& patternName) const;
            int upsertModelPattern(const std::string& patternName, int expectedObjectCount, double referencePointSpread) const;
            int insertModelObjectType(const std::string& objectType) const;

            int getModelPatternId(const std::string& patternName) const;
            int getModelObjectTypeId(const std::string& objectType) const;
            int ensureModelPatternName(const std::string& patternName) const;
            int ensureModelObjectType(const std::string& objectType) const;

            const PatternNameToPatternMap getPatternDefinitionsByName(const std::set<std::string>& patternNames) const;

            const std::set<std::string> getObjectTypes() const;
            const ObjectTypeToVoteMap getVoteSpecifiersForObjectTypes(const std::set<std::string>& objectTypes) const;
    };

    typedef boost::shared_ptr<TableHelper> TableHelperPtr;
}

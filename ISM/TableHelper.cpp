#include "TableHelper.hpp"
#include <set>
#include <iostream>
#include <vector>
#include <map>
#include <boost/foreach.hpp>

namespace ISM {

    /*******************
      UTIL
     *******************/

    TableHelper::TableHelper(std::string dbfilename) {
        this->sqlite.reset(new session(sqlite3, dbfilename.c_str()));
        this->createTablesIfNecessary();
    }

    TableHelper::~TableHelper() {
        (*this->sqlite).close();
    }

    void TableHelper::createTablesIfNecessary() const {
        std::set<std::string> tables;

        rowset<row> rs = (*sqlite).prepare<< "SELECT tbl_name FROM sqlite_master WHERE type = 'table';";
        for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
            row const& row = *it;
            tables.insert(row.get<std::string>(0));
        }

        std::map<std::string, std::string> tableDefs;
        tableDefs["recorded_objects"] = "id INTEGER PRIMARY KEY, type TEXT, observedId TEXT, setId INTEGER, px FLOAT, py FLOAT, pz FLOAT, qw FLOAT, qx FLOAT, qy FLOAT, qz FLOAT";
        tableDefs["recorded_sets"] = "id INTEGER PRIMARY KEY, patternId INTEGER";
        tableDefs["recorded_patterns"] = "id INTEGER PRIMARY KEY, name TEXT UNIQUE";
        tableDefs["model_objects"] = "id INTEGER PRIMARY KEY, type TEXT UNIQUE";
        tableDefs["model_votes"] =
            "id INTEGER PRIMARY KEY, objectId INTEGER, patternId INTEGER, observedId TEXT, radius FLOAT"
            ", qw FLOAT, qx FLOAT, qy FLOAT, qz FLOAT"
            ", qw2 FLOAT, qx2 FLOAT, qy2 FLOAT, qz2 FLOAT"
            ", qpw FLOAT, qpx FLOAT, qpy FLOAT, qpz FLOAT"
            ", qpw2 FLOAT, qpx2 FLOAT, qpy2 FLOAT, qpz2 FLOAT";
        tableDefs["model_patterns"] = "id INTEGER PRIMARY KEY, name TEXT UNIQUE, expectedMaxWeight INTEGER";

        typedef std::pair<std::string, std::string> pair_type;
        BOOST_FOREACH(pair_type p, tableDefs) {
            if (tables.find(p.first) == tables.end()) {
                this->createTable(p.first, p.second);
            }
        }
    }

    void TableHelper::createTable(const std::string& tablename, const std::string& sql) const {
        try {
            (*sqlite).once<<"CREATE TABLE `"<<tablename<<"` ("<<sql<<");";
        } catch (soci_error e) {
            std::cerr<<"SQL error "<<e.what()<<std::endl;
            throw e;
        }
    }

    void TableHelper::dropTable(const std::string& tablename) const {
        try {
            (*sqlite).once<<"DROP TABLE `"<<tablename<<"`;";
        } catch (soci_error e) {
            std::cerr<<"SQL error "<<e.what()<<std::endl;
            throw e;
        }
    }

    void TableHelper::dropModelTables() const {
        this->dropTable("model_votes");
        this->dropTable("model_patterns");
        this->dropTable("model_objects");
    }

    int TableHelper::getLastInsertId(const std::string& tablename) const {
        int id = 0;
        try {
            (*sqlite)<<"SELECT id FROM `"<<tablename<<"` ORDER BY ID DESC LIMIT 1;", into(id);
        } catch (soci_error e) {
            std::cerr<<"SQL error "<<e.what()<<std::endl;
            throw e;
        }

        return id;
    }

    /*******************
      Recorded
     *******************/

    int TableHelper::insertRecordedObject(const boost::shared_ptr<Object>& o, int setId) const {
        (*sqlite) << "INSERT INTO `recorded_objects` (type, observedId, setId, px, py, pz, qw, qx, qy, qz) values (:type, :oid, :setId, :px, :py, :pz, :qw, :qx, :qy, :qz);",
            use(o->type),
            use(o->observedId),
            use(setId),
            use(o->pose->point->x),
            use(o->pose->point->y),
            use(o->pose->point->z),
            use(o->pose->quat->w),
            use(o->pose->quat->x),
            use(o->pose->quat->y),
            use(o->pose->quat->z);

        return this->getLastInsertId("recorded_objects");
    }

    int TableHelper::insertRecordedObjectSet(const ObjectSetPtr& os, const std::string& patternName) const {
        std::vector<boost::shared_ptr<Object> > objects = os->objects;

        transaction trans(*sqlite);
        int patternId = this->ensureRecordedPatternName(patternName);

        (*sqlite) << "INSERT INTO `recorded_sets` (patternId) VALUES (:patternId);", use(patternId);
        int setId = this->getLastInsertId("recorded_sets");
        for (size_t i = 0; i < objects.size(); i++) {
            this->insertRecordedObject(objects[i], setId);
        }

        trans.commit();

        return setId;
    }

    int TableHelper::insertRecordedPattern(const std::string& patternName) const {
        (*sqlite) << "INSERT INTO `recorded_patterns` (name) VALUES (:patternName);", use(patternName);
        return this->getLastInsertId("recorded_patterns");
    }

    int TableHelper::ensureRecordedPatternName(const std::string& patternName) const {
        int id = this->getRecordedPatternId(patternName);
        return id == 0 ? this->insertRecordedPattern(patternName) : id;
    }

    std::vector<std::string> TableHelper::getRecordedPatternNames() const {
        std::vector<std::string> names;
        rowset<std::string> rs = ((*sqlite).prepare<< "SELECT name FROM `recorded_patterns`;");
        for (auto& name : rs) {
            names.push_back(name);;
        }

        return names;
    }

    int TableHelper::getRecordedPatternId(const std::string& patternName) const {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM `recorded_patterns` WHERE name = :name;", into(id, ind), use(patternName);
        if ((*sqlite).got_data() && ind == i_ok) {
            return id;
        } else {
            return 0;
        }
    }

    const RecordedPatternPtr TableHelper::getRecordedPattern(const std::string& patternName) const {
        boost::shared_ptr<RecordedPattern> pattern;
        int patternId = this->getRecordedPatternId(patternName);

        if (patternId != 0) {
            pattern.reset(new RecordedPattern(patternName));

            rowset<int> rs = ((*sqlite).prepare<< "SELECT id FROM `recorded_sets` WHERE patternId = :patternId;", use(patternId));
            for (rowset<int>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
                pattern->addObjectSet(this->getRecordedObjectSet(*it));
            }
        }

        return pattern;
    }

    const ObjectSetPtr TableHelper::getRecordedObjectSet(int setId) const {
        boost::shared_ptr<ObjectSet> s(new ObjectSet());

        rowset<row> rs = ((*sqlite).prepare<<
            "SELECT type, observedId, px, py, pz, qw, qx, qy, qz FROM `recorded_objects` WHERE setId = :setId;",
            use(setId)
        );

        for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
            row const& row = *it;
            s->insert(
                ObjectPtr(
                    new Object(
                        row.get<std::string>(0, ""),
                        new Pose(
                            new Point(
                                row.get<double>(2, 0.0),
                                row.get<double>(3, 0.0),
                                row.get<double>(4, 0.0)
                            ),
                            new Quaternion(
                                row.get<double>(5, 0.0),
                                row.get<double>(6, 0.0),
                                row.get<double>(7, 0.0),
                                row.get<double>(8, 0.0)
                            )
                        ),
                        row.get<std::string>(1, "")
                    )
                )
            );
        }

        return s;
    }

    /*******************
      Model
     *******************/

    int TableHelper::insertModelVoteSpecifier(const VoteSpecifierPtr& vote) const {
        int patternId = this->ensureModelPatternName(vote->patternName);
        int objectId = this->ensureModelObjectType(vote->objectType);

        (*sqlite) << "INSERT INTO `model_votes` "<<
            "(objectId, patternId, observedId, radius, qw, qx, qy, qz, qw2, qx2, qy2, qz2, qpw, qpx, qpy, qpz, qpw2, qpx2, qpy2, qpz2) values "<<
            "(:objectId, :patternId, :observedId, :radius, :qw, :qx, :qy, :qz, :qw2, :qx2, :qy2, :qz2, :qpw, :qpx, :qpy, :qpz, :qpw2, :qpx2, :qpy2, :qpz2);",
            use(objectId),
            use(patternId),
            use(vote->observedId),
            use(vote->radius),
            use(vote->objectToRefQuat->w),
            use(vote->objectToRefQuat->x),
            use(vote->objectToRefQuat->y),
            use(vote->objectToRefQuat->z),
            use(vote->objectToRefPoseQuat->w),
            use(vote->objectToRefPoseQuat->x),
            use(vote->objectToRefPoseQuat->y),
            use(vote->objectToRefPoseQuat->z),
            use(vote->refToObjectQuat->w),
            use(vote->refToObjectQuat->x),
            use(vote->refToObjectQuat->y),
            use(vote->refToObjectQuat->z),
            use(vote->refToObjectPoseQuat->w),
            use(vote->refToObjectPoseQuat->x),
            use(vote->refToObjectPoseQuat->y),
            use(vote->refToObjectPoseQuat->z);

        return this->getLastInsertId("model_votes");
    }

    int TableHelper::ensureModelPatternName(const std::string& patternName) const {
        int id = this->getModelPatternId(patternName);
        return id == 0 ? this->insertModelPattern(patternName) : id;
    }

    int TableHelper::ensureModelObjectType(const std::string& objectType) const {
        int id = this->getModelObjectTypeId(objectType);
        return id == 0 ? this->insertModelObjectType(objectType) : id;
    }

    int TableHelper::insertModelPattern(const std::string& patternName) const {
        (*sqlite) << "INSERT INTO `model_patterns` (name) VALUES (:patternName);", use(patternName);
        return this->getLastInsertId("model_patterns");
    }

    int TableHelper::upsertModelPattern(const std::string& patternName, int expectedMaxWeight) const {
        int patternId = this->getModelPatternId(patternName);
        if (patternId == 0) {
            (*sqlite) << "INSERT INTO `model_patterns` (name, expectedMaxWeight) VALUES (:patternName, :expectedMaxWeight);",
                use(patternName),
                use(expectedMaxWeight);
        } else {
            (*sqlite) << "REPLACE INTO `model_patterns` (id, name, expectedMaxWeight) VALUES (:id, :patternName, :expectedMaxWeight);",
                use(patternId),
                use(patternName),
                use(expectedMaxWeight);
        }
        return this->getLastInsertId("model_patterns");
    }

    int TableHelper::insertModelObjectType(const std::string& objectType) const {
        (*sqlite) << "INSERT INTO `model_objects` (type) VALUES (:objectType);", use(objectType);
        return this->getLastInsertId("model_objects");
    }

    int TableHelper::getModelPatternId(const std::string& patternName) const {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM `model_patterns` WHERE name = :name;", into(id, ind), use(patternName);
        if ((*sqlite).got_data() && ind == i_ok) {
            return id;
        } else {
            return 0;
        }
    }

    int TableHelper::getModelObjectTypeId(const std::string& objectType) const {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM `model_objects` WHERE type = :objectType;", into(id, ind), use(objectType);
        if ((*sqlite).got_data() && ind == i_ok) {
            return id;
        } else {
            return 0;
        }
    }

    const PatternNameToPatternMap TableHelper::getPatternDefinitionsByName(const std::set<std::string>& patternNames) const {
        PatternNameToPatternMap patterns;
        BOOST_FOREACH(std::string patternName, patternNames) {
            int expectedMaxWeight;
            indicator indEOC;
            (*sqlite) <<
                "SELECT expectedMaxWeight "<<
                "FROM `model_patterns` "<<
                "WHERE name = :patternName LIMIT 1;",
                into(expectedMaxWeight, indEOC),
                use(patternName);
            if ((*sqlite).got_data() && indEOC == i_ok) {
                patterns[patternName] = PatternPtr(new Pattern(patternName, expectedMaxWeight));
            }
        }

        return patterns;
    }

    const std::set<std::string> TableHelper::getObjectTypes() const {
        rowset<row> rs = ((*sqlite).prepare<<"SELECT type FROM `model_objects`;");
        std::set<std::string> types;
        BOOST_FOREACH(row const& row, rs) {
            types.insert(row.get<std::string>(0));
        }
        return types;
    }

    const std::set<std::pair<std::string, std::string> > TableHelper::getObjectTypesAndIds() const {
        rowset<row> rs = ((*sqlite).prepare<<"SELECT DISTINCT type, observedId FROM model_votes v JOIN model_objects o on o.id = v.objectId");
        std::set<std::pair<std::string, std::string> > typesAndIds;
        BOOST_FOREACH(row const& row, rs) {
            typesAndIds.insert(std::make_pair(row.get<std::string>(0), row.get<std::string>(1)));
        }
        return typesAndIds;
    }

    const ObjectTypeToVoteMap TableHelper::getVoteSpecifiersForObjectTypes(const std::set<std::string>& objectTypes) const {
        ObjectTypeToVoteMap voteSpecifierMap;
        BOOST_FOREACH(std::string objectType, objectTypes) {
            int objectId = getModelObjectTypeId(objectType);
            rowset<row> rs = ((*sqlite).prepare<<
                "SELECT radius, name, type, observedId, qw, qx, qy, qz, qw2, qx2, qy2, qz2, qpw, qpx, qpy, qpz, qpw2, qpx2, qpy2, qpz2 "<<
                "FROM `model_votes` AS v "<<
                "JOIN `model_objects` AS o ON v.objectId = o.id "<<
                "JOIN `model_patterns` AS p ON v.patternId = p.id "<<
                "WHERE objectId = :objectId;",
                use(objectId)
            );
            std::vector<VoteSpecifierPtr> specifiers;
            BOOST_FOREACH(row const& row, rs) {
                VoteSpecifierPtr vote(
                    new VoteSpecifier(
                        QuaternionPtr(
                            new Quaternion(
                                row.get<double>(4, 0.0),
                                row.get<double>(5, 0.0),
                                row.get<double>(6, 0.0),
                                row.get<double>(7, 0.0)
                            )
                        ),
                        QuaternionPtr(
                            new Quaternion(
                                row.get<double>(8, 0.0),
                                row.get<double>(9, 0.0),
                                row.get<double>(10, 0.0),
                                row.get<double>(11, 0.0)
                            )
                        ),
                        QuaternionPtr(
                            new Quaternion(
                                row.get<double>(12, 0.0),
                                row.get<double>(13, 0.0),
                                row.get<double>(14, 0.0),
                                row.get<double>(15, 0.0)
                            )
                        ),
                        QuaternionPtr(
                            new Quaternion(
                                row.get<double>(16, 0.0),
                                row.get<double>(17, 0.0),
                                row.get<double>(18, 0.0),
                                row.get<double>(19, 0.0)
                            )
                        ),
                        row.get<double>(0, 0.0),
                        row.get<std::string>(1, ""),
                        row.get<std::string>(2, ""),
                        row.get<std::string>(3, "")
                    )
                );
                specifiers.push_back(vote);
            }
            voteSpecifierMap[objectType] = specifiers;
        };

        return voteSpecifierMap;
    }
}

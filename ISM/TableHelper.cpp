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

    void TableHelper::createTablesIfNecessary() {
        std::set<std::string> tables;

        rowset<row> rs = (*sqlite).prepare<< "SELECT tbl_name FROM sqlite_master WHERE type = 'table';";
        for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
            row const& row = *it;
            std::cout<<"found table "<<row.get<std::string>(0)<<std::endl;
            tables.insert(row.get<std::string>(0));
        }

        std::map<std::string, std::string> tableDefs;
        tableDefs["recorded_objects"] = "id INTEGER PRIMARY KEY, type TEXT, observedId TEXT, setId INTEGER, px FLOAT, py FLOAT, pz FLOAT, qw FLOAT, qx FLOAT, qy FLOAT, qz FLOAT";
        tableDefs["recorded_sets"] = "id INTEGER PRIMARY KEY, patternId INTEGER";
        tableDefs["recorded_patterns"] = "id INTEGER PRIMARY KEY, name TEXT UNIQUE";
        tableDefs["model_objects"] = "id INTEGER PRIMARY KEY, type TEXT UNIQUE";
        tableDefs["model_votes"] = "id INTEGER PRIMARY KEY, objectId INTEGER, patternId INTEGER, observedId TEXT, radius FLOAT, qw FLOAT, qx FLOAT, qy FLOAT, qz FLOAT";
        tableDefs["model_patterns"] = "id INTEGER PRIMARY KEY, name TEXT UNIQUE";

        typedef std::pair<std::string, std::string> pair_type;
        BOOST_FOREACH(pair_type p, tableDefs) {
            if (tables.find(p.first) == tables.end()) {
                this->createTable(p.first, p.second);
            }
        }
    }

    void TableHelper::createTable(std::string tablename, std::string sql) {
        std::cout<<"creating "<<tablename<<" table"<<std::endl;
        try {
            (*sqlite).once<<"CREATE TABLE `"<<tablename<<"` ("<<sql<<");";
        } catch (soci_error e) {
            std::cerr<<"SQL error "<<e.what()<<std::endl;
            throw e;
        }
    }

    int TableHelper::getLastInsertId(std::string tablename) {
        int id = 0;
        try {
            (*sqlite)<<"SELECT id FROM "<<tablename<<" ORDER BY ID DESC LIMIT 1;", into(id);
        } catch (soci_error e) {
            std::cerr<<"SQL error "<<e.what()<<std::endl;
            throw e;
        }

        return id;
    }

    /*******************
      Recorded
     *******************/

    int TableHelper::insertRecordedObject(boost::shared_ptr<Object> o, int setId) {
        (*sqlite) << "INSERT INTO `recorded_objects` (type, observedId, setId, px, py, pz, qw, qx, qy, qz) values (:type, :oid, :setId, :px, :py, :pz, :qw, :qx, :qy, :qz);",
            use(o->getType()),
            use(o->getID()),
            use(setId),
            use(o->getPose()->point->x),
            use(o->getPose()->point->y),
            use(o->getPose()->point->z),
            use(o->getPose()->quat->w),
            use(o->getPose()->quat->x),
            use(o->getPose()->quat->y),
            use(o->getPose()->quat->z);

        return this->getLastInsertId("recorded_objects");
    }

    int TableHelper::insertRecordedObjectSet(boost::shared_ptr<ObjectSet> os, std::string patternName) {
        std::vector<boost::shared_ptr<Object> > objects = os->getObjects();

        int patternId = this->ensureRecordedPatternName(patternName);

        (*sqlite) << "INSERT INTO `recorded_sets` (patternId) VALUES (:patternId);", use(patternId);
        int setId = this->getLastInsertId("recorded_sets");
        for (size_t i = 0; i < objects.size(); i++) {
            this->insertRecordedObject(objects[i], setId);
        }

        return setId;
    }

    int TableHelper::insertRecordedPattern(std::string patternName) {
        (*sqlite) << "INSERT INTO `recorded_patterns` (name) VALUES (:patternName);", use(patternName);
        return this->getLastInsertId("recorded_patterns");
    }

    int TableHelper::ensureRecordedPatternName(std::string patternName) {
        int id = this->getRecordedPatternId(patternName);
        return id == 0 ? this->insertRecordedPattern(patternName) : id;
    }

    int TableHelper::getRecordedPatternId(std::string patternName) {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM `recorded_patterns` WHERE name = :name;", into(id, ind), use(patternName);
        if ((*sqlite).got_data() && ind == i_ok) {
            return id;
        } else {
            return 0;
        }
    }

    boost::shared_ptr<RecordedPattern> TableHelper::getRecordedPattern(std::string patternName) {
        boost::shared_ptr<RecordedPattern> pattern;
        int patternId = this->getRecordedPatternId(patternName);

        if (patternId != 0) {
            pattern.reset(new RecordedPattern(patternName));

            rowset<int> rs = ((*sqlite).prepare<< "SELECT id FROM `recorded_sets` WHERE patternId = :patternId;", use(patternId));
            for (rowset<int>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
                std::cout<<"found set id: "<<*it<<std::endl;
                pattern->addObjectSet(this->getRecordedObjectSet(*it));
            }
        }

        return pattern;
    }

    boost::shared_ptr<ObjectSet> TableHelper::getRecordedObjectSet(int setId) {
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
                        row.get<std::string>(1, ""),
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
                        )
                    )
                )
            );
        }

        return s;
    }

    /*******************
      Model
     *******************/

    int TableHelper::ensureModelPatternName(std::string patternName) {
        int id = this->getModelPatternId(patternName);
        return id == 0 ? this->insertModelPattern(patternName) : id;
    }

    int TableHelper::insertModelPattern(std::string patternName) {
        (*sqlite) << "INSERT INTO `model_patterns` (name) VALUES (:patternName);", use(patternName);
        return this->getLastInsertId("model_patterns");
    }

    int TableHelper::getModelPatternId(std::string patternName) {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM `model_patterns` WHERE name = :name;", into(id, ind), use(patternName);
        if ((*sqlite).got_data() && ind == i_ok) {
            return id;
        } else {
            return 0;
        }
    }
}

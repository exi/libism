#include "TableHelper.hpp"
#include <set>
#include <iostream>
#include <vector>

namespace ISM {
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

        if (tables.find(std::string("objects")) == tables.end()) {
            this->createObjectsTable();
        }

        if (tables.find(std::string("sets")) == tables.end()) {
            this->createSetsTable();
        }

        if (tables.find(std::string("patterns")) == tables.end()) {
            this->createPatternsTable();
        }
    }

    void TableHelper::createObjectsTable() {
        this->createTable(
            "objects",
            "CREATE TABLE objects (id INTEGER PRIMARY KEY, type TEXT, observedId TEXT, setId FLOAT, px FLOAT, py FLOAT, pz FLOAT, qx FLOAT, qy FLOAT, qz FLOAT, qw FLOAT);"
        );
    }

    void TableHelper::createSetsTable() {
        this->createTable(
            "sets",
            "CREATE TABLE sets (id INTEGER PRIMARY KEY, patternId INTEGER);"
        );
    }

    void TableHelper::createPatternsTable() {
        this->createTable(
            "patterns",
            "CREATE TABLE patterns (id INTEGER PRIMARY KEY, name TEXT UNIQUE);"
        );
    }

    void TableHelper::createTable(std::string tablename, std::string sql) {
        std::cout<<"creating "<<tablename<<" table"<<std::endl;
        try {
            (*sqlite).once<<sql;
        } catch (soci_error e) {
            std::cerr<<"SQL error "<<e.what()<<std::endl;
            throw e;
        }
    }

    int TableHelper::insertObject(boost::shared_ptr<Object> o, int setId) {
        (*sqlite) << "INSERT INTO objects(type, observedId, setId, px, py, pz, qx, qy, qz, qw) values (:type, :oid, :setId, :px, :py, :pz, :qx, :qy, :qz, :qw);",
            use(o->getType()),
            use(o->getID()),
            use(setId),
            use(o->getPose().point.x),
            use(o->getPose().point.y),
            use(o->getPose().point.z),
            use(o->getPose().quat.x),
            use(o->getPose().quat.y),
            use(o->getPose().quat.z),
            use(o->getPose().quat.w);

        return this->getLastInsertId("objects");
    }

    int TableHelper::insertObjectSet(boost::shared_ptr<ObjectSet> os, std::string patternName) {
        std::vector<boost::shared_ptr<Object> > objects = os->getObjects();

        int patternId = this->ensurePatternName(patternName);

        (*sqlite) << "INSERT INTO sets(patternId) VALUES (:patternId);", use(patternId);
        int setId = this->getLastInsertId("sets");
        for (size_t i = 0; i < objects.size(); i++) {
            this->insertObject(objects[i], setId);
        }

        return setId;
    }

    int TableHelper::insertPattern(std::string patternName) {
        (*sqlite) << "INSERT INTO patterns(name) VALUES (:patternName);", use(patternName);
        return this->getLastInsertId("patterns");
    }

    int TableHelper::ensurePatternName(std::string patternName) {
        int id = this->getPatternId(patternName);
        return id == 0 ? this->insertPattern(patternName) : id;
    }

    int TableHelper::getPatternId(std::string patternName) {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM patterns WHERE name = :name;", into(id, ind), use(patternName);
        if ((*sqlite).got_data() && ind == i_ok) {
            return id;
        } else {
            return 0;
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

    boost::shared_ptr<RecordedPattern> TableHelper::getRecordedPattern(std::string patternName) {
        boost::shared_ptr<RecordedPattern> pattern;
        int patternId = this->getPatternId(patternName);

        if (patternId != 0) {
            pattern.reset(new RecordedPattern(patternName));

            rowset<int> rs = ((*sqlite).prepare<< "SELECT id FROM sets WHERE patternId = :patternId;", use(patternId));
            for (rowset<int>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
                std::cout<<"found set id: "<<*it<<std::endl;
                pattern->addObjectSet(this->getObjectSet(*it));
            }
        }

        return pattern;
    }

    boost::shared_ptr<ObjectSet> TableHelper::getObjectSet(int setId) {
        boost::shared_ptr<ObjectSet> s(new ObjectSet());

        rowset<row> rs = ((*sqlite).prepare<<
            "SELECT type, observedId, px, py, pz, qx, qy, qz, qw FROM objects WHERE setId = :setId;",
            use(setId)
        );

        for (rowset<row>::const_iterator it = rs.begin(); it != rs.end(); ++it) {
            row const& row = *it;
            s->insert(boost::shared_ptr<Object>(new Object(
                row.get<std::string>(0, ""),
                row.get<std::string>(1, ""),
                Pose(
                    Point(
                        row.get<double>(2, 0.0),
                        row.get<double>(3, 0.0),
                        row.get<double>(4, 0.0)
                    ),
                    Quaternion(
                        row.get<double>(5, 0.0),
                        row.get<double>(6, 0.0),
                        row.get<double>(7, 0.0),
                        row.get<double>(8, 0.0)
                    )
                )
            )));
        }

        return s;
    }
}

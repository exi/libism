#include "TableHelper.hpp"
#include <set>
#include <iostream>
#include <vector>

namespace ISM {
    TableHelper::TableHelper(std::string dbfilename) {
        this->sqlite.reset(new session(sqlite3, dbfilename.c_str()));
    }

    void TableHelper::createTablesIfNecessary() {
        std::set<std::string> tables;

        rowset<row> rs = (*sqlite).prepare<< "select tbl_name from sqlite_master";
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

        if (tables.find(std::string("object2set")) == tables.end()) {
            this->createObject2SetTable();
        }

        if (tables.find(std::string("scenes")) == tables.end()) {
            this->createScenesTable();
        }
    }

    void TableHelper::createObjectsTable() {
        this->createTable(
                "objects",
                "CREATE TABLE objects (id INTEGER PRIMARY KEY, type TEXT, observedId NUMERIC, px NUMERIC, py NUMERIC, pz NUMERIC, qx NUMERIC, qy NUMERIC, qz NUMERIC, qw NUMERIC);"
                );
    }

    void TableHelper::createSetsTable() {
        this->createTable(
                "sets",
                "CREATE TABLE sets (id INTEGER PRIMARY KEY, sceneId INTEGER);"
                );
    }

    void TableHelper::createObject2SetTable() {
        this->createTable(
                "object2set",
                "CREATE TABLE object2set (id INTEGER PRIMARY KEY, objectId INTEGER, setId INTEGER);"
                );
    }

    void TableHelper::createScenesTable() {
        this->createTable(
                "scenes",
                "CREATE TABLE scenes (id INTEGER PRIMARY KEY, name TEXT UNIQUE);"
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

    int TableHelper::insertObject(boost::shared_ptr<Object> o) {
        (*sqlite) << "INSERT INTO objects(type, observedId, px, py, pz, qx, qy, qz, qw) values (:type, :oid, :px, :py, :pz, :qx, :qy, :qz, :qw);",
            use(o->getType()),
            use(o->getID()),
            use(o->getPose().point.x),
            use(o->getPose().point.y),
            use(o->getPose().point.z),
            use(o->getPose().quat.x),
            use(o->getPose().quat.y),
            use(o->getPose().quat.z),
            use(o->getPose().quat.w);

        return this->getLastInsertId("objects");
    }

    int TableHelper::insertObjectSet(boost::shared_ptr<ObjectSet> os, std::string sceneName) {
        std::vector<boost::shared_ptr<Object> > objects = os->getObjects();

        int sceneId = this->ensureSceneName(sceneName);

        (*sqlite) << "INSERT INTO sets(sceneId) VALUES (:sceneId);", use(sceneId);
        int setId = this->getLastInsertId("sets");
        for (size_t i = 0; i < objects.size(); i++) {
            int oid = this->insertObject(objects[i]);
            (*sqlite) << "INSERT INTO object2set(objectId, setId) VALUES (:objectId, :setId);", use(oid), use(setId);
        }

        return setId;
    }

    int TableHelper::insertScene(std::string sceneName) {
        (*sqlite) << "INSERT INTO scenes(name) VALUES (:sceneName);", use(sceneName);
        return this->getLastInsertId("scenes");
    }

    int TableHelper::ensureSceneName(std::string sceneName) {
        int id;
        indicator ind;
        (*sqlite) << "SELECT id FROM scenes WHERE name = :name LIMIT 1;", into(id, ind), use(sceneName);
        if (ind == i_ok) {
            return id;
        } else {
            return this->insertScene(sceneName);
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
}

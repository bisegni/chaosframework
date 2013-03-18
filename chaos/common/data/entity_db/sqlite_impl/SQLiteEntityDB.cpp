/*
 *	SQLiteEntityDB.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */


#include "SQLiteEntityDB.h"

using namespace chaos;
using namespace chaos::edb;

#define SET_CHECK_AND_TROW_ERROR(var, sql, st) \
var=sqlite3_prepare_v2(dbInstance, sql, -1, &st, NULL);\
if(var != SQLITE_OK) throw CException(var, sqlite3_errstr(var), "SQLiteEntityDB::initDB");

#define DONE_TO_OK_RESULT(result, step_func) (result=step_func)==SQLITE_DONE?SQLITE_OK:result

#define CREATE_SEQ_TABLE \
"create table if not exists seq (seq_num integer, seq_name char(32))"

#define CREATE_KEY_TABLE \
"create table if not exists key (id integer primary key , key_name char(256))"

#define CREATE_ENTITY_TABLE \
"create table if not exists entity (id integer primary key , key_id integer, key_type integer, str_value char(256), num_value integer,  double_value double, FOREIGN KEY(key_id) REFERENCES key(id) ON DELETE RESTRICT)"

#define CREATE_PROPERTY_TABLE \
"create table if not exists property (id integer primary key , entity_id integer, key_id integer,  key_type integer, str_value char(256), num_value integer,  double_value double, FOREIGN KEY(entity_id) REFERENCES entity(id) ON DELETE CASCADE, FOREIGN KEY(key_id) REFERENCES key(id) ON DELETE RESTRICT)"

#define CREATE_ENTITY_RELATION_TABLE \
"create table if not exists entity_group (id_entity_parent integer, id_entity_child integer, primary key (id_entity_parent, id_entity_child), FOREIGN KEY(id_entity_parent) REFERENCES entity(id) ON DELETE CASCADE, FOREIGN KEY(id_entity_child) REFERENCES entity(id) ON DELETE RESTRICT)"

/*!
 Default constructor
 */
SQLiteEntityDB::SQLiteEntityDB(/*string *_alias*/) {
    //alias = *_alias;
}

/*!
 Default destructor
 */
SQLiteEntityDB::~SQLiteEntityDB() {
    
}


/*!
 Initialize the db implementation
 */
int16_t SQLiteEntityDB::initDB(const char* name, bool temporary)  throw (CException) {
    int16_t result = 0;
    int errorSequence = 0;
    std::string uriPath = "file:";
    uriPath.append(name);
    
    //init superclass
    EntityDB::initDB(name, temporary);
    
    if((_temporaryAllocation)) {
        //use mermory database for temporary
        uriPath.append("?mode=memory&cache=shared");
    }
    
    //clean all statement pointer
    for (int i = 0; i < NUM_STMT; i++) {
        stmt[i] = NULL;
    }
    
    //open database
    if(openDatabase(uriPath.c_str()) != SQLITE_OK) throw CException(errorSequence++, "Error openening database", "SQLiteEntityDB::initDB");
    
    //create tables
    if(makeInsertUpdateDelete(CREATE_SEQ_TABLE) != SQLITE_DONE) throw CException(1, "Error initializing sequence table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_KEY_TABLE) != SQLITE_DONE) throw CException(2, "Error initializing key table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_TABLE) != SQLITE_DONE) throw CException(3, "Error initializing entity table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_TABLE) != SQLITE_DONE) throw CException(4, "Error initializing property table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_RELATION_TABLE) != SQLITE_DONE) throw CException(5, "Error initializing entity table relation", "SQLiteEntityDB::initDB");
    
    
    // create increment update statement on table name
    SET_CHECK_AND_TROW_ERROR(result, "update seq set seq_num = seq_num + 1 where seq_name = ?", stmt[0])
    SET_CHECK_AND_TROW_ERROR(result, "select seq_num from seq where seq_name = ?", stmt[1])
    SET_CHECK_AND_TROW_ERROR(result, "insert into key values (?,?)", stmt[2])
    SET_CHECK_AND_TROW_ERROR(result, "select id from key where key_name = ?", stmt[3])
    SET_CHECK_AND_TROW_ERROR(result, "insert into entity(id, key_id, key_type, str_value) values (?,?,?,?)", stmt[4])
    SET_CHECK_AND_TROW_ERROR(result, "insert into entity(id, key_id,  key_type, num_value) values (?,?,?,?)", stmt[5])
    SET_CHECK_AND_TROW_ERROR(result, "select id from entity where key_id = ? and str_value like ?", stmt[6])
    SET_CHECK_AND_TROW_ERROR(result, "select id from entity where key_id = ? and num_value = ?", stmt[7])
    SET_CHECK_AND_TROW_ERROR(result, "select distinct(entity_id) from property where key_id = ? and str_value like ?    ", stmt[8])
    SET_CHECK_AND_TROW_ERROR(result, "select distinct(entity_id) from property where key_id = ? and num_value = ?", stmt[9])
    SET_CHECK_AND_TROW_ERROR(result, "select distinct(entity_id) from property where key_id = ? and double_value = ?", stmt[10])
    SET_CHECK_AND_TROW_ERROR(result, "delete from entity where id = ?", stmt[11])
    
    //-----property
    SET_CHECK_AND_TROW_ERROR(result, "insert into property(id, entity_id, key_id, key_type, str_value) values (?,?,?,?,?)", stmt[12])
    SET_CHECK_AND_TROW_ERROR(result, "insert into property(id, entity_id, key_id, key_type, num_value) values (?,?,?,?,?)", stmt[13])
    SET_CHECK_AND_TROW_ERROR(result, "insert into property(id, entity_id, key_id, key_type, double_value) values (?,?,?,?,?)", stmt[14])
    SET_CHECK_AND_TROW_ERROR(result, "update property set str_value = ?, entity_id = ? where id = ?", stmt[15])
    SET_CHECK_AND_TROW_ERROR(result, "update property set num_value = ?, entity_id = ? where id = ?", stmt[16])
    SET_CHECK_AND_TROW_ERROR(result, "update property set double_value = ?, entity_id = ? where id = ?", stmt[17])
    SET_CHECK_AND_TROW_ERROR(result, "delete from property where id = ?", stmt[18])
    
    //search
    SET_CHECK_AND_TROW_ERROR(result, "select key_id, key_type, CASE (key_type) when 0 then num_value when 1 then double_value else str_value end from property where entity_id = ?", stmt[19])
    SET_CHECK_AND_TROW_ERROR(result, "select id from entity where key_id = ?", stmt[20])
    
    SET_CHECK_AND_TROW_ERROR(result, "delete from property where entity_id = ?", stmt[21])
    
    //entity group
    SET_CHECK_AND_TROW_ERROR(result, "insert into entity_group values (?,?)", stmt[22])
    SET_CHECK_AND_TROW_ERROR(result, "delete from entity_group where id_entity_parent = ? and id_entity_child = ?", stmt[23])
    SET_CHECK_AND_TROW_ERROR(result, "select count(*) from entity_group where id_entity_parent = ? and id_entity_child = ?", stmt[24])
    SET_CHECK_AND_TROW_ERROR(result, "select id_entity_child from entity_group where id_entity_parent = ?", stmt[25])
    
    SET_CHECK_AND_TROW_ERROR(result, "select key_id, key_type, CASE (key_type) when 0 then num_value when 1 then double_value else str_value end from entity where id = ?", stmt[26])
    SET_CHECK_AND_TROW_ERROR(result, "delete from entity_group where id_entity_parent = ? ", stmt[27])
    SET_CHECK_AND_TROW_ERROR(result, "select id from entity e, entity_group g where e.id = g.id_entity_child and e.key_id = ? and e.str_value like ? and g.id_entity_parent = ?", stmt[28])
    SET_CHECK_AND_TROW_ERROR(result, "select id from entity e, entity_group g where e.id = g.id_entity_child and e.key_id = ? and e.num_value = ? and g.id_entity_parent = ?", stmt[29])
    SET_CHECK_AND_TROW_ERROR(result, "select id from entity e, entity_group g where e.id = g.id_entity_child and e.key_id = ? and e.double_value = ? and g.id_entity_parent = ?", stmt[30])
    
    //init the sequence
    if(!result && !hasSequence("key")){
        result = initSequence("key");
    }
    
    if(!result && !hasSequence("entity")){
        result = initSequence("entity");
    }
    
    if(!result && !hasSequence("property")){
        result = initSequence("property");
    }
    return result;
}

/*!
 Initialize the db implementation
 */
int16_t SQLiteEntityDB::deinitDB()  throw (CException) {
    int16_t result = 0;
    for (int i = 0; i < NUM_STMT; i++) {
        if(stmt[i]) sqlite3_finalize(stmt[i]);
        stmt[i] = NULL;
    }
    
    closeDatabase();
    return result;
}


/*!
 add a new Key returning the associated ID.
 */
int16_t SQLiteEntityDB::getIDForKey(const char *newKey, uint32_t& keyID) {
    int16_t result = 0;
    
    sqlite3_reset(stmt[3]);
    sqlite3_reset(stmt[2]);
    
    result = sqlite3_bind_text(stmt[3], 1, newKey, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_step(stmt[3]);
    if(result != SQLITE_ROW && result != SQLITE_DONE) return result;
    
    //get the id
    if(result == SQLITE_ROW) {
        keyID = sqlite3_column_int(stmt[3], 0);
        return SQLITE_OK;
    }
    
    //get next id
    if((result =  getNextIDOnTable("key", keyID)) != SQLITE_OK) return result;
    
    //
    result = sqlite3_bind_int(stmt[2], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_text(stmt[2], 2, newKey, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_step(stmt[2]);
    if(result != SQLITE_DONE) return result;
    
    return  SQLITE_OK;
}

/*
 add a new entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::getIDForEntity(KeyIdAndValue& keyInfo, uint32_t& newEntityID) {
    int16_t result = 0;
    sqlite3_stmt *targetStmt = NULL;
    std::vector<uint32_t> resultEntityIDs;
    
    result = searchEntityByKeyAndValue(keyInfo, resultEntityIDs);
    if(result != SQLITE_OK) return result;
    
    if(resultEntityIDs.size()== 1) {
        newEntityID = resultEntityIDs[0];
        return result;
    }

    switch (keyInfo.type) {
        case KEY_NUM_VALUE:
            targetStmt = stmt[5];
            sqlite3_reset(targetStmt);
            result = sqlite3_bind_int64(targetStmt, 4, keyInfo.value.numValue);
            if(result != SQLITE_OK) return result;
            break;
        case KEY_DOUBLE_VALUE:
            //not managed
            return -1;
            break;
        case KEY_STR_VALUE: {
            targetStmt = stmt[4];
            sqlite3_reset(targetStmt);
            result = sqlite3_bind_text(targetStmt, 4, keyInfo.value.strValue, -1, NULL);
            if(result != SQLITE_OK) return result;
            break;
        }
    }
    
    //get next id
    if((result =  getNextIDOnTable("entity", newEntityID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(targetStmt, 1, newEntityID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int64(targetStmt, 2, keyInfo.keyID);
    if(result != SQLITE_OK) return result;
    
    // set key type
    result = sqlite3_bind_int(targetStmt, 3, keyInfo.type);
    if(result != SQLITE_OK) return result;
    
    //insert the entity
    result = sqlite3_step(stmt[4]);
    if(result != SQLITE_DONE) return result;
    
    return  SQLITE_OK;
}

/*
 Attach an entity to another
 */
int16_t SQLiteEntityDB::attachEntityChildToEntityParent(uint32_t parentEntity, uint32_t childEntity) {
    int16_t result = 0;
    sqlite3_reset(stmt[22]);
    
    // set parent
    result = sqlite3_bind_int(stmt[22], 1, parentEntity);
    if(result != SQLITE_OK) return result;
    
    // set child
    result = sqlite3_bind_int(stmt[22], 2, childEntity);
    if(result != SQLITE_OK) return result;
    
    //insert
    result = sqlite3_step(stmt[22]);
    if(result != SQLITE_DONE) return result;
    
    return  SQLITE_OK;

}

/*
 Remove an attache entity entity to another
 */
int16_t SQLiteEntityDB::removeEntityChildFromEntityParent(uint32_t parentEntity, uint32_t childEntity) {
    int16_t result = 0;
    sqlite3_reset(stmt[23]);
    
    // set parent
    result = sqlite3_bind_int(stmt[23], 1, parentEntity);
    if(result != SQLITE_OK) return result;
    
    //set child
    result = sqlite3_bind_int(stmt[23], 2, childEntity);
    if(result != SQLITE_OK) return result;
    
    
    //Delete
    result = sqlite3_step(stmt[23]);
    if(result != SQLITE_DONE) return result;
    
    return  SQLITE_OK;
}

/*!
 */
int16_t SQLiteEntityDB::removeAllEntityChild(uint32_t parentEntity) {
    int16_t result = 0;
    sqlite3_reset(stmt[27]);
    
    // set parent
    result = sqlite3_bind_int(stmt[27], 1, parentEntity);
    if(result != SQLITE_OK) return result;
    
    //Delete
    result = sqlite3_step(stmt[27]);
    if(result != SQLITE_DONE) return result;
    
    return  SQLITE_OK;
}

/*
 Check if the parent and child are joined togheter
 */
int16_t SQLiteEntityDB::checkParentChildJoined(uint32_t parentEntity, uint32_t childEntity, bool& joined) {
    int16_t result = 0;
    sqlite3_reset(stmt[24]);
    
    //set parent
    result = sqlite3_bind_int(stmt[24], 1, parentEntity);
    if(result != SQLITE_OK) return result;
    
    //set child
    result = sqlite3_bind_int(stmt[24], 2, childEntity);
    if(result != SQLITE_OK) return result;
    
    //search
    if( (result = sqlite3_step(stmt[24])) == SQLITE_ROW ){
        joined = (sqlite3_column_int(stmt[24], 0) == 1);
    } else {
        joined = false;
    }
    
    return SQLITE_OK;
}

/*!
 
 */
int16_t SQLiteEntityDB::getAllChildEntity(uint32_t parentEntity, std::vector<uint32_t>& child) {
    int16_t result = 0;
    sqlite3_reset(stmt[25]);
    
    //set parent
    result = sqlite3_bind_int(stmt[25], 1, parentEntity);
    if(result != SQLITE_OK) return result;

    //search
    while( (result = sqlite3_step(stmt[25])) == SQLITE_ROW ) {
        child.push_back(sqlite3_column_int(stmt[25], 0));
    }
    return SQLITE_OK;
}

/*!
 
 */
int16_t SQLiteEntityDB::getEntityKeyInfoByID(uint32_t entityID, KeyIdAndValue& keyInfo) {
    int16_t result = 0;
    sqlite3_reset(stmt[26]);
    
    //set parent
    result = sqlite3_bind_int(stmt[26], 1, entityID);
    if(result != SQLITE_OK) return result;
    
    //search
    while( (result = sqlite3_step(stmt[26])) == SQLITE_ROW ) {
        fillKeyInfoFromStatement(stmt[26], keyInfo);
    }
    return SQLITE_OK;
}

/*!
 Delete the entity and all associated property
 */
int16_t SQLiteEntityDB::deleteEntity(uint32_t entityID) {
    int16_t result = 0;
    sqlite3_reset(stmt[11]);

    // set new entity id
    result = sqlite3_bind_int(stmt[11], 1, entityID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_step(stmt[11]);
    if(result != SQLITE_OK) return result;
    
    return  SQLITE_OK;
}

/*
 add a new number property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(uint32_t entityID, KeyIdAndValue& keyInfo, uint32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    sqlite3_stmt *targetStmt = NULL;
        
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    switch (keyInfo.type) {
        case KEY_NUM_VALUE:
            targetStmt = stmt[13];
            sqlite3_reset(targetStmt);

            // set key id
            result = sqlite3_bind_int64(targetStmt, 5, keyInfo.value.numValue);
            break;
        case KEY_DOUBLE_VALUE:
            //not managed
            targetStmt = stmt[14];
            sqlite3_reset(targetStmt);

            result = sqlite3_bind_double(targetStmt, 5, keyInfo.value.doubleValue);
            break;
        case KEY_STR_VALUE: {
            targetStmt = stmt[12];
            sqlite3_reset(targetStmt);

            result = sqlite3_bind_text(targetStmt, 5, keyInfo.value.strValue, -1, NULL);
            break;
        }
    }
    if(result != SQLITE_OK) return result;
    
    
    // set new entity id
    result = sqlite3_bind_int(targetStmt, 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(targetStmt, 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int64(targetStmt, 3, keyInfo.keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_int(targetStmt, 4, keyInfo.type);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_step(targetStmt);
    if(result != SQLITE_OK) return result;
    
    return  SQLITE_OK;
}

/*
 update the integer value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(uint32_t propertyID, KeyIdAndValue& newTypeAndValue) {
    int16_t result = 0;
    
    sqlite3_stmt *targetStmt = NULL;
        
    switch (newTypeAndValue.type) {
        case KEY_NUM_VALUE:
            targetStmt = stmt[16];
            sqlite3_reset(targetStmt);

            // set key id
            result = sqlite3_bind_int64(targetStmt, 1, newTypeAndValue.value.numValue);
            break;
        case KEY_DOUBLE_VALUE:
            //not managed
            targetStmt = stmt[17];
            sqlite3_reset(targetStmt);

            result = sqlite3_bind_double(targetStmt, 1, newTypeAndValue.value.doubleValue);
            break;
        case KEY_STR_VALUE: {
            targetStmt = stmt[15];
            sqlite3_reset(targetStmt);

            result = sqlite3_bind_text(targetStmt, 1, newTypeAndValue.value.strValue, -1, NULL);
            break;
        }
    }
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_int64(targetStmt, 2, newTypeAndValue.keyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(targetStmt, 3, propertyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_step(targetStmt);
    if(result != SQLITE_OK) return result;

    return  SQLITE_OK;
    
}


/*
 search the entitys with key and value
 */
int16_t SQLiteEntityDB::searchEntityByKeyAndValue(KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) {
    int16_t result = 0;
    
    bool onlyKeyID = keyInfo.value.numValue == 0;
    sqlite3_stmt *targetStmt = NULL;
    
    sqlite3_reset(stmt[7]);
    sqlite3_reset(stmt[6]);
    
    if(onlyKeyID){
        targetStmt = stmt[20];
        sqlite3_reset(targetStmt);
    }else{
        switch (keyInfo.type) {
            case KEY_NUM_VALUE:
                targetStmt = stmt[7];
                sqlite3_reset(targetStmt);
                result = sqlite3_bind_int64(targetStmt, 2, keyInfo.value.numValue);
                if(result != SQLITE_OK) return result;
                break;
            case KEY_DOUBLE_VALUE:
                //not managed
                return -1;
                break;
            case KEY_STR_VALUE: {
                targetStmt = stmt[6];
                sqlite3_reset(targetStmt);
                // set key id
                result = sqlite3_bind_text(targetStmt, 2, keyInfo.value.strValue, -1, NULL);
                if(result != SQLITE_OK) return result;
                break;
            }
        }
    }
    // set new entity id
    result = sqlite3_bind_int(targetStmt, 1, keyInfo.keyID);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(targetStmt)) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int(targetStmt, 0));
    }
    
    return  SQLITE_OK;
    
}

/*!
 search the entitys with key and value
 */
int16_t SQLiteEntityDB::searchEntityByParentIDAndKeyValue(uint32_t parentID, KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) {
    int16_t result = 0;
    
    sqlite3_stmt *targetStmt = NULL;
        switch (keyInfo.type) {
            case KEY_STR_VALUE: {
                targetStmt = stmt[28];
                sqlite3_reset(targetStmt);
                    // set key id
                result = sqlite3_bind_text(targetStmt, 2, keyInfo.value.strValue, -1, NULL);
                if(result != SQLITE_OK) return result;
                break;
            }
            case KEY_NUM_VALUE:
                targetStmt = stmt[29];
                sqlite3_reset(targetStmt);
                result = sqlite3_bind_int64(targetStmt, 2, keyInfo.value.numValue);
                if(result != SQLITE_OK) return result;
                break;
            case KEY_DOUBLE_VALUE:
                targetStmt = stmt[30];
                sqlite3_reset(targetStmt);
                result = sqlite3_bind_double(targetStmt, 2, keyInfo.value.doubleValue);
                if(result != SQLITE_OK) return result;
                break;
            
        }
    
        // set new entity id
    result = sqlite3_bind_int(stmt[28], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_int(stmt[28], 3, parentID);
    if(result != SQLITE_OK) return result;
    
        // cicle the row
    while((result = sqlite3_step(stmt[28])) == SQLITE_ROW) {
            //get found ids
        resultEntityIDs.push_back(sqlite3_column_int(stmt[28], 0));
    }
    
    return  SQLITE_OK;
    

}

/*
 search the entitys using property key and value
 */
int16_t SQLiteEntityDB::searchEntityByPropertyKeyAndValue(KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) {
    int16_t result = 0;
    
    sqlite3_stmt *targetStmt = NULL;
    
    switch (keyInfo.type) {
        case KEY_NUM_VALUE:
            targetStmt = stmt[9];
            sqlite3_reset(targetStmt);
            
            // set key id
            result = sqlite3_bind_int64(targetStmt, 2, keyInfo.value.numValue);
            if(result != SQLITE_OK) return result;
            break;
        case KEY_DOUBLE_VALUE:
            //not managed
            targetStmt = stmt[10];
            sqlite3_reset(targetStmt);
            result = sqlite3_bind_double(targetStmt, 2, keyInfo.value.doubleValue);
            if(result != SQLITE_OK) return result;
            break;
        case KEY_STR_VALUE: {
            targetStmt = stmt[8];
            sqlite3_reset(targetStmt);
            result = sqlite3_bind_text(targetStmt, 2, keyInfo.value.strValue, -1, NULL);
            if(result != SQLITE_OK) return result;
            break;
        }
    }
    
    // set new entity id
    result = sqlite3_bind_int64(targetStmt, 1, keyInfo.keyID);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(targetStmt)) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int(targetStmt, 0));
    }
    
    return  SQLITE_OK;
}

/*
 update the integer value for a property of an entity
 */
int16_t SQLiteEntityDB::searchPropertyForEntity(uint32_t entityID, chaos::ArrayPointer<KeyIdAndValue>& resultKeyAndValues) {
    int16_t result = 0;
    
    sqlite3_reset(stmt[19]);
    
    //set the entity id
    result = sqlite3_bind_int64(stmt[19], 1, entityID);
    if(result != SQLITE_OK) return result;
    
    // cicle the selectted row
    while((result = sqlite3_step(stmt[19])) == SQLITE_ROW) {
        KeyIdAndValue *record = new KeyIdAndValue();
        fillKeyInfoFromStatement(stmt[19], *record);
        resultKeyAndValues.add(record);
    }
    return result;
}

/*
 update the integer value for a property of an entity
 */
int16_t SQLiteEntityDB::searchPropertyForEntity(uint32_t entityID, std::vector<uint32_t>& keysIDs, chaos::ArrayPointer<KeyIdAndValue>& resultKeyAndValues) {
    int16_t result = 0;
    
    sqlite3_stmt *stmt = NULL;

    //if no key id is passed use the other function
    if(keysIDs.size() == 0) return searchPropertyForEntity(entityID, resultKeyAndValues);
    
    // i have key id
    std::string select("select key_id, key_type, CASE (key_type) when 0 then num_result when 1 then double_result else str_result end from property where entity_id = ? and key_id in (");
    
    for(int i = 0; i< keysIDs.size(); i++) {select.append("?,");}
    
    //remove last "," form select
    keysIDs.resize(keysIDs.size()-1);
    
    
    //close select
    select.append(")");
    
    try {
        
        result = sqlite3_prepare_v2(dbInstance, select.c_str(), -1, &stmt, NULL);
        
        // cicle the selectted row
        while((result = sqlite3_step(stmt)) == SQLITE_ROW) {
            KeyIdAndValue *record = new KeyIdAndValue();
            fillKeyInfoFromStatement(stmt, *record);
            resultKeyAndValues.add(record);
        }
        
    } catch (...) {
        
    }
    //clear the statement
    if(stmt) result = sqlite3_finalize(stmt);
    return result;
}

/*!
 Delete a property for a entity
 */
int16_t SQLiteEntityDB::deleteProperty(uint32_t propertyID) {
    int16_t result = 0;
    sqlite3_reset(stmt[18]);
    // set key id
    result = sqlite3_bind_int(stmt[18], 1, propertyID);
    if(result != SQLITE_OK) return result;
    
    return DONE_TO_OK_RESULT(result, sqlite3_step(stmt[18]));
}

/*
 Delete all property for an entity
 */
int16_t SQLiteEntityDB::deleteAllPropertyForEntity(uint32_t entityID) {
    int16_t result = 0;
    sqlite3_reset(stmt[21]);
    // set key id
    result = sqlite3_bind_int(stmt[21], 1, entityID);
    if(result != SQLITE_OK) return result;
    
    return DONE_TO_OK_RESULT(result, sqlite3_step(stmt[21]));
}
//----------------------private db api------------------------

int16_t SQLiteEntityDB::openDatabase(const char *databasePath) {
	int16_t err = sqlite3_open(databasePath, &dbInstance);
	return err;
}

int16_t  SQLiteEntityDB::closeDatabase() {
	int err = SQLITE_OK;
	if(dbInstance)  err = sqlite3_close(dbInstance);
    return err;
}

int16_t SQLiteEntityDB::makeInsertUpdateDelete(const char *sql) {
	int16_t error = SQLITE_OK;
	sqlite3_stmt *insertStmt = NULL;
    if((error = sqlite3_prepare_v2(dbInstance, sql, -1, &insertStmt, NULL)) != SQLITE_OK) {
        return error;
    }
    error = sqlite3_step(insertStmt);
    sqlite3_finalize(insertStmt);
    return error;
}

int16_t SQLiteEntityDB::getNextIDOnTable(const char * tableName, uint32_t &seqID) {
    int16_t error = SQLITE_OK;
    mutex::scoped_lock  lock(seqWorkMutext);
    sqlite3_reset(stmt[0]);
    sqlite3_reset(stmt[1]);
    
    //set table name
    if((error = sqlite3_bind_text(stmt[0], 1, tableName, -1, NULL)) != SQLITE_OK) {
        return error;
    }
    
    
    // update the sequence
    if((error = sqlite3_step(stmt[0])) != SQLITE_DONE) {
        return error;
    }
    
    sqlite3_bind_text(stmt[1], 1, tableName, -1, NULL);
    if((error = sqlite3_step(stmt[1])) != SQLITE_ROW) {
        return error;
    }
    
    seqID = sqlite3_column_int(stmt[1], 0);
    return SQLITE_OK;
}

int16_t SQLiteEntityDB::initSequence(const char *tableName) {
    int16_t error = SQLITE_OK;
    sqlite3_stmt *insertStmt = NULL;
    if((error = sqlite3_prepare_v2(dbInstance, "insert into seq values (?,?)", -1, &insertStmt, NULL)) != SQLITE_OK) {
        return error;
    }
    
    if((error = sqlite3_bind_text(insertStmt, 2, tableName, -1, NULL)) != SQLITE_OK) {
        sqlite3_finalize(insertStmt);
        return error;
    }
    
    if((error = sqlite3_bind_int(insertStmt, 1, 0)) != SQLITE_OK) {
        sqlite3_finalize(insertStmt);
        return error;
    }
    
    error = sqlite3_step(insertStmt);
    sqlite3_finalize(insertStmt);
    return 0;
}

bool SQLiteEntityDB::hasSequence(const char *tableName) {
    int16_t error = SQLITE_OK;
    int result = 0;
    sqlite3_stmt *hasStmt = NULL;
    if((error = sqlite3_prepare_v2(dbInstance, "select count(*) from seq where seq_name = ?", -1, &hasStmt, NULL)) != SQLITE_OK) {
        return error;
    }
    
    if((error = sqlite3_bind_text(hasStmt, 1, tableName, -1, NULL)) != SQLITE_OK) {
        sqlite3_finalize(hasStmt);
        return error;
    }
    
    error = sqlite3_step(hasStmt);
    result = sqlite3_column_int(hasStmt, 0);
    sqlite3_finalize(hasStmt);
    return result >= 1;
}

void SQLiteEntityDB::fillKeyInfoFromStatement(sqlite3_stmt *stmt, KeyIdAndValue& keyValueInfo) {
    //get the id for the key
    keyValueInfo.keyID = sqlite3_column_int(stmt, 0);
    
    // valorize the need field
    switch (sqlite3_column_int(stmt, 1)) {
        case 0:
            keyValueInfo.value.numValue = sqlite3_column_int64(stmt, 2);
            break;
        case 1:
            keyValueInfo.value.doubleValue = sqlite3_column_int64(stmt, 2);
            break;
        case 2:
            //copy the string into memoery of the value
            strcpy(keyValueInfo.value.strValue, (const char *)sqlite3_column_text(stmt, 2));
            break;
    }
}
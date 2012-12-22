//
//  SQLiteEntityDB.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 21/12/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#include "SQLiteEntityDB.h"

using namespace chaos;
using namespace chaos::edb;

#define CREATE_SEQ_TABLE \
"create table if not exists seq (seq_num integer, seq_name char(32))"

#define CREATE_KEY_TABLE \
"create table if not exists key (id integer primary key , key_name char(256))"

#define CREATE_ENTITY_TABLE \
"create table if not exists entity (id integer primary key , key_id integer, str_value char(256), num_value integer,  double_value double)"

#define CREATE_ENTITY_KEY_FK \
"ALTER TABLE entity ADD CONSTRAINT fk_entity_key FOREIGN KEY (key_id) REFERENCES key(id)"

#define CREATE_PROPERTY_TABLE \
"create table if not exists property (id integer primary key , entity_id integer, key_id integer, str_value char(256), num_value integer,  double_value double)"

#define CREATE_PROPERTY_ENTITY_FK \
"ALTER TABLE property ADD CONSTRAINT fk_prop_ent FOREIGN KEY (entity_id) REFERENCES entity(id)"

#define CREATE_PROPERTY_KEY_FK \
"ALTER TABLE property ADD CONSTRAINT fk_prop_key FOREIGN KEY (key_id) REFERENCES key(id)"

#define CREATE_ENTITY_RELATION_TABLE \
"create table if not exists entity (id_entity_parent integer primary key, id_entity_child integer primary key)"


/*
 sqlite3_column_int(stmt, 0);
 (const char *)sqlite3_column_text(stmt, 1);
 */

/*!
 Initialize the db implementation
 */
int16_t SQLiteEntityDB::initDB(const char* name, bool temporary)  throw (CException) {
    int16_t result = 0;
    EntityDB::initDB(name, temporary);
    
    //open database
    if(openDatabase(name) != SQLITE_OK) throw CException(0, "Error openening database", "SQLiteEntityDB::initDB");
    
    //create tables
    if(makeInsertUpdateDelete(CREATE_SEQ_TABLE) != SQLITE_OK) throw CException(1, "Error initializing sequence table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_KEY_TABLE) != SQLITE_OK) throw CException(2, "Error initializing key table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_TABLE) != SQLITE_OK) throw CException(3, "Error initializing entity table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_KEY_FK) != SQLITE_OK) throw CException(4, "Error initializing entity table fk", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_TABLE) != SQLITE_OK) throw CException(5, "Error initializing property table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_ENTITY_FK) != SQLITE_OK) throw CException(6, "Error initializing property table fk", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_KEY_FK) != SQLITE_OK) throw CException(7, "Error initializing property table fk", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_RELATION_TABLE) != SQLITE_OK) throw CException(8, "Error initializing entity table relation", "SQLiteEntityDB::initDB");
    
    
    // create increment update statement on table name
    result = sqlite3_prepare_v2(dbInstance, "update seq set seq_num = seq_num + 1 where seq_name = ?", -1, &seqIncrementStmt, NULL);
    if(result != SQLITE_OK) throw CException(9, "error initializing seqIncrementStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select seq_num from seq where seq_name = ?", -1, &seqSelectStmt, NULL);
    if(result != SQLITE_OK) throw CException(10, "error initializing seqSelectStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into key values (?,?)", -1, &keyCreationStmt, NULL);
    if(result != SQLITE_OK) throw CException(11, "error initializing keyCreationStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select id from key where key_name = ?", -1, &keyIdSelectionStmt, NULL);
    if(result != SQLITE_OK) throw CException(12, "error initializing keyIdSelectionStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into entity(id, key_id, str_value) values (?,?,?)", -1, &entityCreationStmt, NULL);
    if(result != SQLITE_OK) throw CException(13, "error initializing entityCreationStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "delete entity where id = ?", -1, &entityDeleteStmt, NULL);
    if(result != SQLITE_OK) throw CException(14, "error initializing entityDeleteStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into property(id, entity_id, key_id, str_value) values (?,?,?,?)", -1, &propertyAddStrStmt, NULL);
    if(result != SQLITE_OK) throw CException(15, "error initializing propertyAddStrStmt statement", "SQLiteEntityDB::initDB");

    result = sqlite3_prepare_v2(dbInstance, "insert into property(id, entity_id, key_id, num_value) values (?,?,?,?)", -1, &propertyAddNumberStmt, NULL);
    if(result != SQLITE_OK) throw CException(16, "error initializing propertyAddNumberStmt statement", "SQLiteEntityDB::initDB");

    result = sqlite3_prepare_v2(dbInstance, "insert into property(id, entity_id, key_id, double_value) values (?,?,?,?)", -1, &propertyAddDoubleStmt, NULL);
    if(result != SQLITE_OK) throw CException(17, "error initializing propertyAddDoubleStmt statement", "SQLiteEntityDB::initDB");

    result = sqlite3_prepare_v2(dbInstance, "update property set str_value = ? where id = ?", -1, &propertyUpdateStrStmt, NULL);
    if(result != SQLITE_OK) throw CException(15, "error initializing propertyAddStrStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "update property set num_value = ? where id = ?", -1, &propertyUpdateNumberStmt, NULL);
    if(result != SQLITE_OK) throw CException(16, "error initializing propertyAddNumberStmt statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "update property set double_value = ? where id = ?", -1, &propertyUpdateDoubleStmt, NULL);
    if(result != SQLITE_OK) throw CException(17, "error initializing propertyAddDoubleStmt statement", "SQLiteEntityDB::initDB");
    return result;
}

/*!
 Initialize the db implementation
 */
int16_t SQLiteEntityDB::deinitDB()  throw (CException) {
    int16_t result = 0;
    if(seqIncrementStmt) sqlite3_finalize(seqIncrementStmt);
    if(seqSelectStmt) sqlite3_finalize(seqSelectStmt);
    if(keyCreationStmt) sqlite3_finalize(keyCreationStmt);
    if(keyIdSelectionStmt) sqlite3_finalize(keyIdSelectionStmt);
    if(entityCreationStmt) sqlite3_finalize(entityCreationStmt);
    if(entityDeleteStmt) sqlite3_finalize(entityDeleteStmt);
    if(propertyAddStrStmt) sqlite3_finalize(propertyAddStrStmt);
    if(propertyAddNumberStmt) sqlite3_finalize(propertyAddNumberStmt);
    if(propertyAddDoubleStmt) sqlite3_finalize(propertyAddDoubleStmt);

    closeDatabase();
    return result;
}


/*!
 add a new Key returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewKey(const char *newKey, int32_t& keyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("key", keyID)) != SQLITE_OK) return result;
    
    //
    result = sqlite3_bind_int(keyCreationStmt, 1, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_text(keyCreationStmt, 2, newKey, -1, NULL);
    if(result != SQLITE_OK) return result;

    return  sqlite3_step(keyCreationStmt);
}


/*!
 add a new entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewEntity(int32_t entityKeyID, const char *keyValue, int32_t& newEntityID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("entity", newEntityID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(entityCreationStmt, 1, newEntityID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(entityCreationStmt, 2, entityKeyID);
    if(result != SQLITE_OK) return result;

    // set key str value
    result = sqlite3_bind_text(entityCreationStmt, 3, keyValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(entityCreationStmt);
}



/*!
 Delete the entity and all associated property
 */
int16_t SQLiteEntityDB::deleteEntity(int32_t entityID) {
    int16_t result = 0;
    // set new entity id
    result = sqlite3_bind_int(entityDeleteStmt, 1, entityID);
    if(result != SQLITE_OK) return result;

    return  sqlite3_step(entityDeleteStmt);
}



/*!
 add a new number property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, int64_t value, int32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(propertyAddNumberStmt, 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(propertyAddNumberStmt, 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int(propertyAddNumberStmt, 3, propertyKeyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_int64(propertyAddNumberStmt, 4, value);
    if(result != SQLITE_OK) return result;
    
   return  sqlite3_step(propertyAddNumberStmt);
}



/*!
 add a new double property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, double value, int32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(propertyAddDoubleStmt, 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(propertyAddDoubleStmt, 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int(propertyAddDoubleStmt, 3, propertyKeyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_double(propertyAddDoubleStmt, 4, value);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(propertyAddDoubleStmt);
}



/*!
 add a new string property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, const char * value, int32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(propertyAddStrStmt, 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(propertyAddStrStmt, 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int(propertyAddStrStmt, 3, propertyKeyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_text(propertyAddStrStmt, 4, value, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(propertyAddStrStmt);
}



/*!
 update the integer value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(int32_t propertyID, int64_t newValue) {
    int16_t result = 0;
     
    // set new entity id
    result = sqlite3_bind_int64(propertyUpdateNumberStmt, 1, newValue);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(propertyUpdateNumberStmt, 2, propertyID);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(propertyUpdateNumberStmt);

}



/*!
 update the double value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(int32_t propertyID, double newValue) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_int64(propertyUpdateDoubleStmt, 1, newValue);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(propertyUpdateDoubleStmt, 2, propertyID);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(propertyUpdateDoubleStmt);
}



/*!
 update the string value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(int32_t propertyID, const char * newValue) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_text(propertyUpdateDoubleStmt, 1, newValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(propertyUpdateDoubleStmt, 2, propertyID);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(propertyUpdateDoubleStmt);

}



/*!
 Delete a property for a entity
 */
int16_t SQLiteEntityDB::deleteEntityProperty(int32_t propertyID) {
    int16_t result = 0;
    return result;
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

int16_t SQLiteEntityDB::getNextIDOnTable(const char * tableName, int32_t &seqID) {
    int16_t error = SQLITE_OK;
    mutex::scoped_lock  lock(nextSeqID);
    CHAOS_ASSERT(seqIncrementStmt)
    CHAOS_ASSERT(dbInstance)
    
    //set table name
    sqlite3_bind_text(seqIncrementStmt, 1, tableName, -1, NULL);
    
    // update the sequence
    if((error = sqlite3_step(seqIncrementStmt)) != SQLITE_ROW) {
        return error;
    }
    
    sqlite3_bind_text(seqSelectStmt, 1, tableName, -1, NULL);
    if((error = sqlite3_step(seqSelectStmt)) != SQLITE_ROW) {
        return error;
    }
    
    seqID = sqlite3_column_int(seqSelectStmt, 0);
    return error;
}

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
    int errorSequence = 0;
    
    //clean all statement pointer
    for (int i = 0; i < 19; i++) {
        stmt[i] = NULL;
    }
    
    EntityDB::initDB(name, temporary);
    
    //open database
    if(openDatabase(name) != SQLITE_OK) throw CException(errorSequence++, "Error openening database", "SQLiteEntityDB::initDB");
    
    //create tables
    if(makeInsertUpdateDelete(CREATE_SEQ_TABLE) != SQLITE_OK) throw CException(errorSequence++, "Error initializing sequence table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_KEY_TABLE) != SQLITE_OK) throw CException(errorSequence++, "Error initializing key table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_TABLE) != SQLITE_OK) throw CException(errorSequence++, "Error initializing entity table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_KEY_FK) != SQLITE_OK) throw CException(errorSequence++, "Error initializing entity table fk", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_TABLE) != SQLITE_OK) throw CException(errorSequence++, "Error initializing property table", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_ENTITY_FK) != SQLITE_OK) throw CException(errorSequence++, "Error initializing property table fk", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_PROPERTY_KEY_FK) != SQLITE_OK) throw CException(errorSequence++, "Error initializing property table fk", "SQLiteEntityDB::initDB");
    if(makeInsertUpdateDelete(CREATE_ENTITY_RELATION_TABLE) != SQLITE_OK) throw CException(errorSequence++, "Error initializing entity table relation", "SQLiteEntityDB::initDB");
    
    
    // create increment update statement on table name
    result = sqlite3_prepare_v2(dbInstance, "update seq set seq_num = seq_num + 1 where seq_name = ?", -1, &stmt[0], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[0] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select seq_num from seq where seq_name = ?", -1, &stmt[1], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[1] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into key values (?,?)", -1, &stmt[2], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[2] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select id from key where key_name = ?", -1, &stmt[3], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[3] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into entity(id, key_id, str_value) values (?,?,?)", -1, &stmt[4], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[4] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into entity(id, key_id, num_value) values (?,?,?)", -1, &stmt[5], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[5] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select id from entity where key_id = ? and str_value like '%?%'", -1, &stmt[6], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[6] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select id from entity where key_id = ? and num_value = ?", -1, &stmt[7], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[7] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select distinct(entity_id) from property where key_id = ? and str_value like '%?%'", -1, &stmt[8], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[7] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select distinct(entity_id) from property where key_id = ? and num_value = ?", -1, &stmt[9], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[7] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "select distinct(entity_id) from property where key_id = ? and double_value = ?", -1, &stmt[10], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[7] statement", "SQLiteEntityDB::initDB");
    
    //-----property
    result = sqlite3_prepare_v2(dbInstance, "delete entity where id = ?", -1, &stmt[11], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[11] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "insert into property(id, entity_id, key_id, str_value) values (?,?,?,?)", -1, &stmt[12], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[12] statement", "SQLiteEntityDB::initDB");

    result = sqlite3_prepare_v2(dbInstance, "insert into property(id, entity_id, key_id, num_value) values (?,?,?,?)", -1, &stmt[13], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[13] statement", "SQLiteEntityDB::initDB");

    result = sqlite3_prepare_v2(dbInstance, "insert into property(id, entity_id, key_id, double_value) values (?,?,?,?)", -1, &stmt[14], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[14] statement", "SQLiteEntityDB::initDB");

    result = sqlite3_prepare_v2(dbInstance, "update property set str_value = ? where id = ?", -1, &stmt[15], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[12] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "update property set num_value = ? where id = ?", -1, &stmt[16], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[13] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "update property set double_value = ? where id = ?", -1, &stmt[17], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[14] statement", "SQLiteEntityDB::initDB");
    
    result = sqlite3_prepare_v2(dbInstance, "delete property where id = ?", -1, &stmt[18], NULL);
    if(result != SQLITE_OK) throw CException(errorSequence++, "error initializing stmt[18] statement", "SQLiteEntityDB::initDB");
    
    return result;
}

/*!
 Initialize the db implementation
 */
int16_t SQLiteEntityDB::deinitDB()  throw (CException) {
    int16_t result = 0;
    for (int i = 0; i < 19; i++) {
        if(stmt[i]) sqlite3_finalize(stmt[i]);
        stmt[i] = NULL;
    }

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
    result = sqlite3_bind_int(stmt[2], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_text(stmt[2], 2, newKey, -1, NULL);
    if(result != SQLITE_OK) return result;

    return  sqlite3_step(stmt[2]);
}


/*!
 add a new entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewEntity(int32_t keyID, const char *keyValue, int32_t& newEntityID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("entity", newEntityID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[4], 1, newEntityID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[4], 2, keyID);
    if(result != SQLITE_OK) return result;

    // set key str value
    result = sqlite3_bind_text(stmt[4], 3, keyValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[4]);
}

/*!
 add a new entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewEntity(int32_t keyID, int64_t keyValue, int32_t& newEntityID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("entity", newEntityID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[5], 1, newEntityID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[5], 2, keyID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int64(stmt[5], 3, keyValue);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[5]);
}

/*!
 search the entitys with key and value
 */
int16_t SQLiteEntityDB::searchEntityByKeyAndValue(int32_t keyID, const char * keyValue, std::vector<int64_t> resultEntityIDs) {
    int16_t result = 0;
        
    // set new entity id
    result = sqlite3_bind_int(stmt[6], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_text(stmt[6], 2, keyValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(stmt[6])) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int64(stmt[6], 0));
		
    }
    
    return  result;
}

/*!
 search the entitys with key and value
 */
int16_t SQLiteEntityDB::searchEntityByKeyAndValue(int32_t keyID, int64_t keyValue, std::vector<int64_t> resultEntityIDs) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[7], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int64(stmt[7], 2, keyValue);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(stmt[7])) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int64(stmt[7], 0));
    }
    return result;
}


/*!
 search the entitys using property key and value
 */
int16_t SQLiteEntityDB::searchEntityByPropertyKeyAndValue(int32_t keyID, const char * keyValue, std::vector<int64_t> resultEntityIDs) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[8], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_text(stmt[8], 2, keyValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(stmt[8])) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int64(stmt[8], 0));
		
    }
    
    return  result;
}

/*!
 search the entitys using property key and value
 */
int16_t SQLiteEntityDB::searchEntityByPropertyKeyAndValue(int32_t keyID, int64_t keyValue, std::vector<int64_t> resultEntityIDs) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[9], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int64(stmt[9], 2, keyValue);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(stmt[9])) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int64(stmt[9], 0));
    }
    return result;
}

/*!
 search the entitys using property key and value
 */
int16_t SQLiteEntityDB::searchEntityByPropertyKeyAndValue(int32_t keyID, double keyValue, std::vector<int64_t> resultEntityIDs) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[10], 1, keyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_double(stmt[10], 2, keyValue);
    if(result != SQLITE_OK) return result;
    
    // cicle the row
    while((result = sqlite3_step(stmt[10])) == SQLITE_ROW) {
        //get foun ids
        resultEntityIDs.push_back(sqlite3_column_int64(stmt[10], 0));
    }
    return result;
}

/*!
 Delete the entity and all associated property
 */
int16_t SQLiteEntityDB::deleteEntity(int32_t entityID) {
    int16_t result = 0;
    // set new entity id
    result = sqlite3_bind_int(stmt[11], 1, entityID);
    if(result != SQLITE_OK) return result;

    return  sqlite3_step(stmt[11]);
}



/*!
 add a new number property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(int32_t entityID, int32_t keyID, int64_t keyValue, int32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[13], 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[13], 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int(stmt[13], 3, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_int64(stmt[13], 4, keyValue);
    if(result != SQLITE_OK) return result;
    
   return  sqlite3_step(stmt[13]);
}



/*!
 add a new double property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(int32_t entityID, int32_t keyID, double keyValue, int32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[14], 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[14], 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int(stmt[14], 3, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_double(stmt[14], 4, keyValue);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[14]);
}



/*!
 add a new string property for entity with his key/value returning the associated ID.
 */
int16_t SQLiteEntityDB::addNewPropertyForEntity(int32_t entityID, int32_t keyID, const char * keyValue, int32_t& newEntityPropertyID) {
    int16_t result = 0;
    
    //get next id
    if((result =  getNextIDOnTable("property", newEntityPropertyID)) != SQLITE_OK) return result;
    
    // set new entity id
    result = sqlite3_bind_int(stmt[12], 1, newEntityPropertyID);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[12], 2, entityID);
    if(result != SQLITE_OK) return result;
    
    // set key str value
    result = sqlite3_bind_int(stmt[12], 3, keyID);
    if(result != SQLITE_OK) return result;
    
    result = sqlite3_bind_text(stmt[12], 4, keyValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[12]);
}



/*!
 update the integer value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(int32_t propertyID, int64_t newValue) {
    int16_t result = 0;
     
    // set new entity id
    result = sqlite3_bind_int64(stmt[16], 1, newValue);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[16], 2, propertyID);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[16]);
}



/*!
 update the double value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(int32_t propertyID, double newValue) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_int64(stmt[17], 1, newValue);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[17], 2, propertyID);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[17]);
}



/*!
 update the string value for a property of an entity
 */
int16_t SQLiteEntityDB::updatePropertyForEntity(int32_t propertyID, const char * newValue) {
    int16_t result = 0;
    
    // set new entity id
    result = sqlite3_bind_text(stmt[17], 1, newValue, -1, NULL);
    if(result != SQLITE_OK) return result;
    
    // set key id
    result = sqlite3_bind_int(stmt[17], 2, propertyID);
    if(result != SQLITE_OK) return result;
    
    return  sqlite3_step(stmt[17]);
}

/*!
 Delete a property for a entity
 */
int16_t SQLiteEntityDB::deleteEntityProperty(int32_t propertyID) {
    int16_t result = 0;
    
    // set key id
    result = sqlite3_bind_int(stmt[18], 1, propertyID);
    if(result != SQLITE_OK) return result;

    return sqlite3_step(stmt[17]);;
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
    CHAOS_ASSERT(stmt[0])
    CHAOS_ASSERT(dbInstance)
    
    //set table name
    sqlite3_bind_text(stmt[0], 1, tableName, -1, NULL);
    
    // update the sequence
    if((error = sqlite3_step(stmt[0])) != SQLITE_ROW) {
        return error;
    }
    
    sqlite3_bind_text(stmt[1], 1, tableName, -1, NULL);
    if((error = sqlite3_step(stmt[1])) != SQLITE_ROW) {
        return error;
    }
    
    seqID = sqlite3_column_int(stmt[1], 0);
    return error;
}

//
//  SQLiteEntityDB.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 21/12/12.
//  Copyright (c) 2012 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__SQLiteEntityDB__
#define __CHAOSFramework__SQLiteEntityDB__

#include <boost/thread.hpp>

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/data/entity_db/EntityDB.h>
#include <chaos/common/data/entity_db/sqlite_impl/sqlite3.h>

namespace chaos {
    namespace edb {
        
        /*!
         SQLite implementation for the entity db
         */
        REGISTER_AND_DEFINE_NOALIAS_DERIVED_CLASS_FACTORY(SQLiteEntityDB, EntityDB) {
            sqlite3 *dbInstance;
            
            boost::mutex nextSeqID;
            
            //autoncrement statement
            sqlite3_stmt *seqIncrementStmt;
            sqlite3_stmt *seqSelectStmt;
            
            sqlite3_stmt *keyCreationStmt;
            sqlite3_stmt *keyIdSelectionStmt;
            
            sqlite3_stmt *entityCreationStmt;
            sqlite3_stmt *entityDeleteStmt;
            
            sqlite3_stmt *propertyAddStrStmt;
            sqlite3_stmt *propertyAddNumberStmt;
            sqlite3_stmt *propertyAddDoubleStmt;
            sqlite3_stmt *propertyUpdateStrStmt;
            sqlite3_stmt *propertyUpdateNumberStmt;
            sqlite3_stmt *propertyUpdateDoubleStmt;
            
            
            inline int16_t openDatabase(const char *databasePath);
            
            inline int16_t closeDatabase();
            
            inline int16_t makeInsertUpdateDelete(const char *sql);
            
            inline int16_t getNextIDOnTable(const char * tableName, int32_t &seqID);
            
        public:
            /*!
             Initialize the db implementation
             */
            int16_t initDB(const char* name, bool temporary = true)  throw (CException) ;
            
            /*!
             Initialize the db implementation
             */
            int16_t deinitDB()  throw (CException) ;
            
            /*!
             add a new Key returning the associated ID.
             \param newKey is the string associate to a key
             \param keyID is the new returned ID for the key
             */
            int16_t addNewKey(const char *newKey, int32_t& keyID);
            
            /*!
             add a new entity with his key/value returning the associated ID.
             \param entityKeyID the key id for the entity key
             \param entityKeyName the name associate to the key forthe new entity
             \param newEntityID is the new returned ID for the entity
             */
            int16_t addNewEntity(int32_t entityKeyID, const char *keyValue, int32_t& newEntityID);
            
            /*!
             Delete the entity and all associated property
             \param entityID is the id of the key to delete
             */
            int16_t deleteEntity(int32_t entityID);
            
            /*!
             add a new number property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            int16_t addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, int64_t value, int32_t& newEntityPropertyID);
            
            /*!
             add a new double property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            int16_t addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, double value, int32_t& newEntityPropertyID);
            
            /*!
             add a new string property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            int16_t addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, const char * value, int32_t& newEntityPropertyID);
            
            /*!
             update the integer value for a property of an entity
             \param entityID is the id that identify the entity where to attach the property
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            int16_t updatePropertyForEntity(int32_t propertyID, int64_t newValue);
            
            /*!
             update the double value for a property of an entity
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            int16_t updatePropertyForEntity(int32_t propertyID, double newValue);
            
            /*!
             update the string value for a property of an entity
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            int16_t updatePropertyForEntity(int32_t propertyID, const char * newValue);
            
            /*!
             Delete a property for a entity
             \param propertyID is the id of the property
             */
            int16_t deleteEntityProperty(int32_t propertyID);

        };
    }
}

#endif /* defined(__CHAOSFramework__SQLiteEntityDB__) */

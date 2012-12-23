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
            sqlite3_stmt *stmt[19];
            
            
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
             */
            int16_t addNewKey(const char *newKey, int32_t& keyID);
            
            /*
             add a new entity with his key/value returning the associated ID.
             */
            int16_t addNewEntity(int32_t keyID, const char *keyValue, int32_t& newEntityID);
            
            /*
             add a new entity with his key/value returning the associated ID.
             */
            int16_t addNewEntity(int32_t keyID, int64_t keyValue, int32_t& newEntityID);
            
            /*
             search the entitys with key and value
             */
            int16_t searchEntityByKeyAndValue(int32_t keyID, const char * keyValue, std::vector<int64_t> resultEntityIDs);
            
            /*
             search the entitys with key and value
             */
            int16_t searchEntityByKeyAndValue(int32_t keyID, int64_t keyValue, std::vector<int64_t> resultEntityID);
            
            /*
             search the entitys using property key and value
             */
            int16_t searchEntityByPropertyKeyAndValue(int32_t keyID, const char * keyValue, std::vector<int64_t> resultEntityIDs);
            
            /*
             search the entitys using property key and value
             */
            int16_t searchEntityByPropertyKeyAndValue(int32_t keyID, int64_t keyValue, std::vector<int64_t> resultEntityID);
            
            /*
             search the entitys using property key and value
             */
            int16_t searchEntityByPropertyKeyAndValue(int32_t keyID, double keyValue, std::vector<int64_t> resultEntityID);

            
            /*
             Delete the entity and all associated property
             */
            int16_t deleteEntity(int32_t entityID);
            
            /*
             add a new number property for entity with his key/value returning the associated ID.
             */
            int16_t addNewPropertyForEntity(int32_t entityID, int32_t keyID, int64_t keyValue, int32_t& newEntityPropertyID);
            
            /*
             add a new double property for entity with his key/value returning the associated ID.
             */
            int16_t addNewPropertyForEntity(int32_t entityID, int32_t keyID, double keyValue, int32_t& newEntityPropertyID);
            
            /*
             add a new string property for entity with his key/value returning the associated ID.
             */
            int16_t addNewPropertyForEntity(int32_t entityID, int32_t keyID, const char * keyValue, int32_t& newEntityPropertyID);
            
            /*
             update the integer value for a property of an entity
             */
            int16_t updatePropertyForEntity(int32_t propertyID, int64_t newValue);
            
            /*
             update the double value for a property of an entity
             */
            int16_t updatePropertyForEntity(int32_t propertyID, double newValue);
            
            /*
             update the string value for a property of an entity
             */
            int16_t updatePropertyForEntity(int32_t propertyID, const char * newValue);
            
            /*
             Delete a property for a entity
             */
            int16_t deleteEntityProperty(int32_t propertyID);

        };
    }
}

#endif /* defined(__CHAOSFramework__SQLiteEntityDB__) */

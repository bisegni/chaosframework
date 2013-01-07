/*
 *	SQLiteEntityDB.h
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

#ifndef __CHAOSFramework__SQLiteEntityDB__
#define __CHAOSFramework__SQLiteEntityDB__

#include <boost/thread.hpp>
#include <string>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/data/entity_db/EntityDB.h>
#include <chaos/common/data/entity_db/sqlite_impl/sqlite3.h>

#define NUM_STMT 25

namespace chaos {
    namespace edb {
        
        
        /*!
         SQLite implementation for the entity db
         */
        class SQLiteEntityDB : public EntityDB {
            string alias;
            sqlite3 *dbInstance;
            
            boost::mutex seqWorkMutext;
            
            //autoncrement statement
            sqlite3_stmt *stmt[NUM_STMT];
            
            
            inline int16_t openDatabase(const char *databasePath);
            
            inline int16_t closeDatabase();
            
            inline int16_t makeInsertUpdateDelete(const char *sql);
            
            inline int16_t getNextIDOnTable(const char * tableName, int32_t &seqID);
            
            int16_t initSequence(const char *tableName);
            
            bool hasSequence(const char *tableName);
        public:
            /*!
             Default constructor
             */
            SQLiteEntityDB();

            /*!
             Default destructor
             */
            virtual ~SQLiteEntityDB();
            
            /*
             Initialize the db implementation
             */
            int16_t initDB(const char* name, bool temporary = true)  throw (CException) ;
            
            /*
             Initialize the db implementation
             */
            int16_t deinitDB()  throw (CException) ;
            
            /*
             add a new Key returning the associated ID.
             */
            int16_t getIDForKey(const char *newKey, int32_t& keyID);
            
            /*
             add a new entity with his key/value returning the associated ID.
             */
            int16_t getIDForEntity(KeyIdAndValue& keyInfo, int32_t& newEntityID);
            
            /*
             Attach an entity to another
             */
            int16_t attachEntityChildToEntityParent(int32_t parentEntity, int32_t childEntity);
            
            /*
             Remove an attache entity entity to another
             */
            int16_t removeEntityChildFromEntityParent(int32_t parentEntity, int32_t childEntity);
            /*
             Check if the parent and child are joined togheter
             */
            int16_t checkParentChildJoined(int32_t parentEntity, int32_t childEntity, bool& joined);
            /*
             search the entitys with key and value
             */
            int16_t searchEntityByKeyAndValue(KeyIdAndValue& keyInfo, std::vector<int32_t>& resultEntityIDs);
            
            /*
             search the entitys using property key and value
             */
            int16_t searchEntityByPropertyKeyAndValue(KeyIdAndValue& keyInfo, std::vector<int32_t>& resultEntityIDs);
            
            /*
             Delete the entity and all associated property
             */
            int16_t deleteEntity(int32_t entityID);
            
            /*
             add a new number property for entity with his key/value returning the associated ID.
             */
            int16_t addNewPropertyForEntity(int32_t entityID, KeyIdAndValue& keyInfo, int32_t& newEntityPropertyID);
            
            /*
             update the integer value for a property of an entity
             */
            int16_t updatePropertyForEntity(int32_t propertyID, KeyIdAndValue& newTypeAndValue);
            
            /*
             return all property of an entity
             */
            int16_t searchPropertyForEntity(int32_t entityID, chaos::ArrayPointer<KeyIdAndValue>& resultKeyAndValues);
            
            /*
             eturn al lprorerty for an entity for wich the ids are contained into the keysIDs array
             */
            int16_t searchPropertyForEntity(int32_t entityID, std::vector<int32_t>& keysIDs, chaos::ArrayPointer<KeyIdAndValue>& resultKeyAndValues);

            /*
             Delete a property for a entity
             */
            int16_t deleteProperty(int32_t propertyID);
            
            /*
             Delete all property for an entity
             */
            int16_t deleteAllPropertyForEntity(int32_t entityID);

        };
    }
}

#endif /* defined(__CHAOSFramework__SQLiteEntityDB__) */

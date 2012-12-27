/*
 *	EntityDB.h
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

#ifndef __CHAOSFramework__EntityDB__
#define __CHAOSFramework__EntityDB__

#include <stdint.h>
#include <stddef.h>
#include <string>
#include <vector>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ArrayPointer.h>


namespace chaos {
    
    namespace entity {
        class Entity;
    }
    
    /*!
     The namespace edb group all code for the managment of a the abstract entity database.
     */
    namespace edb {
        
        /*!
         An entity will be
         specialized from other class to realize, device, node, etc. Every entity can have many different property, and is identified by a key and a string value.
         
         A property has a name, a type and a value. A property can have many sub property. Either entity and property are key
         value style. The key is registered into a table where a new id is created so only integer are used for key.
         
         ex:
         Entity-> device::id/sensor1
         property -> attribute_ds/volt
         property -> attribute_type/int32
         property -> attribute_dv/32
         ....etc.....
         
         The entity can be grouped together to create a tree structure
         Entity-> device::id/global_sensor
         Entity-> device::id/sensor1
         Entity-> device::id/sensor2
         ....etc.....
         */
        
        
        class EntityDB {
        protected:
            bool    _temporaryAllocation;
            std::string  _name;
        public:
            
            /*!
             Define the type of the value for a key
             */
            typedef enum {
                NUM_VALUE = 0,
                DOUBLE_VALUE,
                STR_VALUE
            } ValueType;
            
            /*!
             Define an union for contain temporaly, the value for a key
             */
            typedef union {
                int64_t numValue;
                double doubleValue;
                char strValue[256];
            } KeyValue;
            
            /*!
             Define the infromation for the type, id and value for a key
             */
            typedef struct {
                ValueType   type;
                int32_t     keyID;
                KeyValue    value;
            } KeyIdAndValue;
            
            /*!
             Default constructor
             */
            EntityDB();
            
            /*!
             Default destructor
             */
            virtual ~EntityDB();
            
            
            /*!
             Initialize the db implementation
             \name the name of the database
             \param temporary if true instruct the driver to create a temporary database
             that will be deleted when the object will be destroyed
             */
            virtual int16_t initDB(const char* name, bool temporary = true) throw (CException) {
                _name.assign(name);
                _temporaryAllocation = temporary;
                return 0;
            }
            
            /*!
             Initialize the db implementation
             */
            virtual int16_t deinitDB()  throw (CException) = 0;
            
            /*!
             add a new Key returning the associated ID.
             \param newKey is the string associate to a key
             \param keyID is the new returned ID for the key
             */
            virtual int16_t getIDForKey(const char *newKey, int32_t& keyID) = 0;
            
            /*!
             try to find the key and id value on entity, if no entity is found, new one
             is created
             \param keyInfo the key information for the new entity
             \param newEntityID is the new returned ID for the entity
             */
            virtual int16_t getIDForEntity(KeyIdAndValue& keyInfo, int32_t& newEntityID) = 0;
            
            /*!
             search the entitys with key and value
            \param keyInfo the key information for the new entity
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByKeyAndValue(KeyIdAndValue& keyInfo, std::vector<int32_t>& resultEntityIDs) = 0;
            
            /*!
             search the entitys using property key and value
             \param keyInfo is the string associate to a key
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByPropertyKeyAndValue(KeyIdAndValue& keyInfo, std::vector<int32_t>& resultEntityIDs) = 0;
            
            /*!
             Delete the entity and all associated property
             \param entityID is the id of the key to delete
             */
            virtual int16_t deleteEntity(int32_t entityID) = 0;
            
            /*!
             add a new number property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param keyInfo the key information for the new entity
             \param newEntityPropertyID is the new id for the entity/property 
             */
            virtual int16_t addNewPropertyForEntity(int32_t entityID, KeyIdAndValue& keyInfo, int32_t& newEntityPropertyID) = 0;
            
            /*!
             update the integer value for a property of an entity
             \param propertyID is the id of the property
             \param newTypeAndValue is the string associate to a key
             */
            virtual int16_t updatePropertyForEntity(int32_t propertyID, KeyIdAndValue& newTypeAndValue) = 0;
            
            /*!
             return all property of an entity
             \param propertyID is the id of the property
             \param resultKeyAndValues the information of the key found
             */
            virtual int16_t searchPropertyForEntity(int32_t entityID, chaos::ArrayPointer<KeyIdAndValue>& resultKeyAndValues) = 0;
            
            /*!
             return al lprorerty for an entity for wich the ids are contained into the keysIDs array
             \param propertyID is the id of the property
             \param keysIDs the vector that contain the id for the key to that must be returned
             \param resultKeyAndValues the information of the key found
             */
            virtual int16_t searchPropertyForEntity(int32_t entityID, std::vector<int32_t>& keysIDs, chaos::ArrayPointer<KeyIdAndValue>& resultKeyAndValues) = 0;
            
            /*!
             Delete a property for a entity
             \param propertyID is the id of the property
             */
            virtual int16_t deleteProperty(int32_t propertyID) = 0;
            
            /*!
             Delete all property for an entity
             \param entityID is the id of the entity
             */
            virtual int16_t deleteAllPropertyForEntity(int32_t entityID) = 0;
            
            /*!
             return a nre isntance of an Entity class
             */
            entity::Entity* getNewEntityInstance();
            
        };
    }
}

#endif /* defined(__CHAOSFramework__EntityDB__) */

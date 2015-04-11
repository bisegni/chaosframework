/*
 *	EntityDB.h
 *	!CHAOS
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
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ArrayPointer.h>
#include <chaos/common/utility/Atomic.h>
#include <boost/ptr_container/ptr_vector.hpp>
namespace chaos {
    using namespace boost;
    
    namespace entity {
        class Entity;
    }
    
     //!The namespace edb group all code for the managment of a the abstract entity database.
    namespace edb {
        
        
        /*!
         Define the type of the value for a key
         */
        typedef enum {
            KEY_NUM_VALUE = 0,
            KEY_DOUBLE_VALUE,
            KEY_STR_VALUE
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
        typedef struct KeyIdAndValue {
            uint32_t        elementID;
            ValueType       type;
            uint32_t        keyID;
            KeyValue        value;
        } KeyIdAndValue;

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

        public:

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
            virtual int16_t initDB(const char* name, bool temporary = true) throw (CException) = 0;
            
            /*!
             Initialize the db implementation
             */
            virtual int16_t deinitDB()  throw (CException) = 0;
            
            /*!
             add a new Key returning the associated ID.
             \param newKey is the string associate to a key
             \param keyID is the new returned ID for the key
             */
            virtual int16_t getIDForKey(const char *newKey, uint32_t& keyID) = 0;
            
            /*!
             try to find the key and id value on entity, if no entity is found, new one
             is created
             \param keyInfo the key information for the new entity
             \param newEntityID is the new returned ID for the entity
             */
            virtual int16_t getIDForEntity(KeyIdAndValue& keyInfo, uint32_t& newEntityID) = 0;
            
            /*!
             Attach an entity to another
             \param parentEntity parent of the new child entity
             \param childEntity child entity
             */
            virtual int16_t attachEntityChildToEntityParent(uint32_t parentEntity, uint32_t childEntity) = 0;
            
            /*!
             Remove an attache entity entity to another
             \param parentEntity parent of the new child entity
             \param childEntity child entity
             */
            virtual int16_t removeEntityChildFromEntityParent(uint32_t parentEntity, uint32_t childEntity) = 0;
            
            /*!
             */
            virtual int16_t removeAllEntityChild(uint32_t parentEntity) = 0;
            
            /*!
             */
            virtual int16_t getAllChildEntity(uint32_t parentEntity, std::vector<uint32_t>& child) = 0;
            
            /*!
             */
            virtual int16_t getEntityKeyInfoByID(uint32_t entityID, KeyIdAndValue& keyInfo) = 0;
            
            /*!
             Check if the parent and child are joined togheter
            \param parentEntity parent of the new child entity
            \param childEntity child entity
            */
            virtual int16_t checkParentChildJoined(uint32_t parentEntity, uint32_t childEntity, bool& joined) = 0;
            
            /*!
             search the entitys with key and value
            \param keyInfo the key information for the new entity
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByKeyAndValue(KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) = 0;
            
            virtual int16_t searchEntityByParentIDAndKeyID(uint32_t parentID, uint32_t keyID, std::vector<uint32_t>& resultEntityIDs) = 0;
            
            /*!
             search the entitys with key and value
             \param keyInfo the key information for the new entity
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByParentIDAndKeyValue(uint32_t parentID, KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) = 0;
            
            
            /*!
             search the entitys with key and value
             */
            virtual int16_t searchEntityByParentAndPropertyIDAndKeyValue(uint32_t parentID, KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) = 0;
            
            /*!
             search the entitys using property key and value
             \param keyInfo is the string associate to a key
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByPropertyKeyAndValue(KeyIdAndValue& keyInfo, std::vector<uint32_t>& resultEntityIDs) = 0;
            
            /*!
             Delete the entity and all associated property
             \param entityID is the id of the key to delete
             */
            virtual int16_t deleteEntity(uint32_t entityID) = 0;
            
            /*!
             add a new number property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param keyInfo the key information for the new entity
             \param newEntityPropertyID is the new id for the entity/property 
             */
            virtual int16_t addNewPropertyForEntity(uint32_t entityID, KeyIdAndValue& keyInfo, uint32_t& newEntityPropertyID) = 0;
            
            /*!
             update the integer value for a property of an entity
             \param propertyID is the id of the property
             \param newTypeAndValue is the string associate to a key
             */
            virtual int16_t updatePropertyForEntity(uint32_t propertyID, KeyIdAndValue& newTypeAndValue) = 0;
            
            /*!
             return all property of an entity
             \param propertyID is the id of the property
             \param resultKeyAndValues the information of the key found
             */
            virtual int16_t searchPropertyForEntity(uint32_t entityID, ptr_vector<KeyIdAndValue>& resultKeyAndValues) = 0;
            
            /*!
             return al lprorerty for an entity for wich the ids are contained into the keysIDs array
             \param propertyID is the id of the property
             \param keysIDs the vector that contain the id for the key to that must be returned
             \param resultKeyAndValues the information of the key found
             */
            virtual int16_t searchPropertyForEntity(uint32_t entityID, std::vector<uint32_t>& keysIDs, ptr_vector<KeyIdAndValue>& resultKeyAndValues) = 0;
            
            /*!
             Delete a property for a entity
             \param propertyID is the id of the property
             */
            virtual int16_t deleteProperty(uint32_t propertyID) = 0;
            
            /*!
             Delete all property for an entity
             \param entityID is the id of the entity
             */
            virtual int16_t deleteAllPropertyForEntity(uint32_t entityID) = 0;
            
            /*!
             Allocate and return a new instance of an Entity class
             \param keyInfo the information for the key taht will represent the entity
             */
            entity::Entity* getNewEntityInstance(KeyIdAndValue& keyInfo);
            
            /*!
             Delete the isntance of an Entity class
             \param entity the instance to delete
             */
            void deleteEntityInstance(entity::Entity* entity);
            
        };
    }
}
#include <chaos/common/data/entity/Entity.h>
#endif /* defined(__CHAOSFramework__EntityDB__) */

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

namespace chaos {
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
            bool    _temporaryAllocation;
            std::string  _name;
        public:
            
            /*!
             Default constructor
             \param temporary if true instruct the driver to create a temporary database
             that will be deleted when the object will be destroyed
             */
            EntityDB();
            
            /*!
             Default destructor
             */
            virtual ~EntityDB();
            
            
            /*!
             Initialize the db implementation
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
            virtual int16_t addNewKey(const char *newKey, int32_t& keyID) = 0;
            
            /*!
             add a new entity with his key/value returning the associated ID.
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the name associate to the key forthe new entity
             \param newEntityID is the new returned ID for the entity
             */
            virtual int16_t addNewEntity(int32_t entityKeyID, const char *entityKeyValue, int32_t& newEntityID) = 0;
        
            /*!
             add a new entity with his key/value returning the associated ID.
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the name associate to the key forthe new entity
             \param newEntityID is the new returned ID for the entity
             */
            virtual int16_t addNewEntity(int32_t entityKeyID, int64_t entityKeyValue, int32_t& newEntityID) = 0;
            
            /*!
             search the entitys with key and value
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the string associated to the entity in fulltext
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByKeyAndValue(int32_t entityKeyID, const char * entityKeyValue, std::vector<int64_t> resultEntityIDs) = 0;
            
            /*!
             search the entitys with key and value
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the int associated to the entity in fulltext
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByKeyAndValue(int32_t entityKeyID, int64_t entityKeyValue, std::vector<int64_t> resultEntityID) = 0;
            
            /*!
             search the entitys using property key and value
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the string associated to the entity in fulltext
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByPropertyKeyAndValue(int32_t entityKeyID, const char * entityKeyValue, std::vector<int64_t> resultEntityIDs) = 0;
            
            /*!
             search the entitys using property key and value
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the int associated to the entity in fulltext
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByPropertyKeyAndValue(int32_t entityKeyID, int64_t entityKeyValue, std::vector<int64_t> resultEntityID) = 0;
            
            /*!
             search the entitys using property key and value
             \param entityKeyID the key id for the entity key
             \param entityKeyValue the int associated to the entity in fulltext
             \param resultEntityIDs the vector containing the retrived id
             */
            virtual int16_t searchEntityByPropertyKeyAndValue(int32_t entityKeyID, double entityKeyValue, std::vector<int64_t> resultEntityID) = 0;
            
            /*!
             Delete the entity and all associated property
             \param entityID is the id of the key to delete
             */
            virtual int16_t deleteEntity(int32_t entityID) = 0;
            
            /*!
             add a new number property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property 
             */
            virtual int16_t addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, int64_t value, int32_t& newEntityPropertyID) = 0;

            /*!
             add a new double property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            virtual int16_t addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, double value, int32_t& newEntityPropertyID) = 0;
            
            /*!
             add a new string property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            virtual int16_t addNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, const char * value, int32_t& newEntityPropertyID) = 0;
            
            /*!
             update the integer value for a property of an entity 
             \param entityID is the id that identify the entity where to attach the property
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            virtual int16_t updatePropertyForEntity(int32_t propertyID, int64_t newValue) = 0;
            
            /*!
             update the double value for a property of an entity
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            virtual int16_t updatePropertyForEntity(int32_t propertyID, double newValue) = 0;
            
            /*!
             update the string value for a property of an entity
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            virtual int16_t updatePropertyForEntity(int32_t propertyID, const char * newValue) = 0;
            
            /*!
             Delete a property for a entity
             \param propertyID is the id of the property
             */
            virtual int16_t deleteEntityProperty(int32_t propertyID) = 0;
            

            
        };
    }
}

#endif /* defined(__CHAOSFramework__EntityDB__) */

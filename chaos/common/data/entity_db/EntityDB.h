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
            
            /*!
             Initialize the db implementation
             */
            int32_t initDB(void);
            
            /*!
             Initialize the db implementation
             */
            int32_t deinitDB(void);
            
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
             Register a new Key returning the associated ID.
             \param newKey is the string associate to a key
             \param keyID is the new returned ID for the key
             */
            virtual int32_t registerNewKey(const char *newKey, int32_t& keyID) = 0;
            
            /*!
             Register a new entity with his key/value returning the associated ID.
             \param entityKeyID the key id for the entity key
             \param entityKeyName the name associate to the key forthe new entity
             \param newEntityID is the new returned ID for the entity
             */
            virtual int32_t registerNewEntity(int32_t entityKeyID, const char *entityKeyName, int32_t& newEntityID) = 0;
            
            /*!
             Delete the entity and all associated property
             \param entityID is the id of the key to delete
             */
            virtual int32_t deleteEntity(int32_t entityID) = 0;
            
            /*!
             Register a new number property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property 
             */
            virtual int32_t registerNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, int64_t value, int32_t* newEntityPropertyID = NULL) = 0;

            /*!
             Register a new double property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            virtual int32_t registerNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, double value, int32_t* newEntityPropertyID = NULL) = 0;
            
            /*!
             Register a new string property for entity with his key/value returning the associated ID.
             \param entityID is the id that identify the entity where to attach the property
             \param propertyKeyID is the id of the key that we need to attach to the entity
             \param value is the value to attach to the property
             \param newEntityPropertyID is the new id for the entity/property
             */
            virtual int32_t registerNewPropertyForEntity(int32_t entityID, int32_t propertyKeyID, const char * value, int32_t* newEntityPropertyID = NULL) = 0;
            
            /*!
             update the integer value for a property of an entity 
             \param entityID is the id that identify the entity where to attach the property
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            virtual int32_t updatePropertyForEntity(int32_t entityID, int32_t propertyID, int64_t newValue) = 0;
            
            /*!
             update the double value for a property of an entity
             \param entityID is the id that identify the entity where to attach the property
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            virtual int32_t updatePropertyForEntity(int32_t entityID, int32_t propertyID, double newValue) = 0;
            
            /*!
             update the string value for a property of an entity
             \param entityID is the id that identify the entity where to attach the property
             \param propertyID is the id of the property
             \param newValue is the new value
             */
            virtual int32_t updatePropertyForEntity(int32_t entityID, int32_t propertyID, const char * newValue) = 0;
            
            /*!
             Delete a property for a entity
             \param entityID is the id that identify the entity where to attach the property
             \param propertyID is the id of the property
             */
            virtual int32_t deleteEntityProperty(int32_t entityID, int32_t propertyID) = 0;
            

            
        };
    }
}

#endif /* defined(__CHAOSFramework__EntityDB__) */

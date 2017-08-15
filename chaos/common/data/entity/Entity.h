/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#ifndef __CHAOSFramework__Entity__
#define __CHAOSFramework__Entity__

#include <vector>
#include <chaos/common/data/entity/Record.h>
//#include <chaos/common/utility/Atomic.h>
#include <boost/ptr_container/ptr_vector.hpp>

namespace chaos {
    
    using namespace boost;
    
    namespace edb {
        //forward declaration
        class EntityDB;
        struct KeyIdAndValue;
    }
    
        //! Namespace for collecting the entity description used by chaos entity database
    namespace entity {
        /*!
         This represent the base class for the entity
         */
        class Entity  : public Record {
            friend class edb::EntityDB;
            
            uint32_t entityID;
            
            Entity(edb::EntityDB *_database);

            
            int32_t setEntityKeyAndInfo(chaos::edb::KeyIdAndValue& keyInfo);
        public:
            
            ~Entity();
            
            int32_t addChild(Entity& entityChild);
            
            int32_t isChild(Entity& entityChild, bool& isChild);
            
            int32_t removeChild(Entity& entityChild);
            
            int32_t removeAllChild();
            
            int32_t getAllChild(ptr_vector<Entity>& childs);
            
            int32_t getChildsWithKeyID(uint32_t keyID, ptr_vector<Entity>& childs);
            
            int32_t getChildWithPropertyKeyIDandValue(edb::KeyIdAndValue& keyInfo, ptr_vector<Entity>& childs);
            
            int32_t getHasChildByKeyInfo(edb::KeyIdAndValue& keyInfo, bool& hasChild);
            
            int32_t getChildsWithKeyInfo(edb::KeyIdAndValue& keyInfo, ptr_vector<Entity>& childs);
            
            int32_t addProperty(uint32_t keyID, const char * propertyVal);
            
            int32_t updateProperty(uint32_t propertyID, uint32_t keyID, const char * propertyVal);

            int32_t addProperty(uint32_t keyID, const std::string& propertyVal);

            int32_t updateProperty(uint32_t propertyID, uint32_t keyID, const std::string& propertyVal);

            int32_t addProperty(uint32_t keyID, int64_t propertyVal);
            
            int32_t updateProperty(uint32_t propertyID, uint32_t keyID, int64_t propertyVal);

            int32_t addProperty(uint32_t keyID, double propertyVal);
            
            int32_t updateProperty(uint32_t propertyID, uint32_t keyID, double propertyVal);

            int32_t resetAllProperty();
			
			int32_t deleteProrperty(uint32_t propertyID);
            
            int32_t getAllProperty(ptr_vector<chaos::edb::KeyIdAndValue>& propertys);
            
            int32_t getPropertyByKeyID(uint32_t keyID, ptr_vector<chaos::edb::KeyIdAndValue>& propertys);
            
            int32_t getPropertyByKeyID(std::vector<uint32_t>& keyIDs, ptr_vector<chaos::edb::KeyIdAndValue>& propertys);
        };
        
    }
    
}

#endif /* defined(__CHAOSFramework__Entity__) */

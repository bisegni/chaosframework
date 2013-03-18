/*
 *	Entity.h
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

#ifndef __CHAOSFramework__Entity__
#define __CHAOSFramework__Entity__

#include <vector>
#include <chaos/common/data/entity/Record.h>
#include <chaos/common/utility/Atomic.h>
namespace chaos {

    namespace edb {
        //forward declaration
        class EntityDB;
        struct KeyIdAndValue;
    }
    
    namespace entity {
       
        
        /*!
         This represent the base class for the entity
         */
        class Entity  : protected Record {
            friend class edb::EntityDB;
            
            atomic_int_type instanceID;
            
            uint32_t entityID;
            
            Entity(edb::EntityDB *_database, atomic_int_type _instanceID);

            
            int32_t setEntityKeyAndInfo(chaos::edb::KeyIdAndValue& keyInfo);
        public:
            
            ~Entity();
            
            int32_t addChild(Entity& entityChild);
            
            int32_t removeChild(Entity& entityChild);
            
            int32_t removeAllChild();
            
            int32_t getAllChild(vector<Entity*>& childs);
            
            int32_t getChildsWithKeyID(uint32_t keyID, vector<Entity*>& childs);
            
            int32_t getHasChildByKeyInfo(edb::KeyIdAndValue& keyInfo, bool& hasChild);
            
            int32_t getChildsWithKeyInfo(edb::KeyIdAndValue& keyInfo, vector<Entity*>& childs);
            
            int32_t addProperty(uint32_t keyID, string& propertyVal);

            int32_t addProperty(uint32_t keyID, int64_t propertyVal);
            
            int32_t addProperty(uint32_t keyID, double propertyVal);
            
            int32_t reset();
            
            int32_t getAllProperty(chaos::ArrayPointer<chaos::edb::KeyIdAndValue>& propertys);
            
            int32_t getPropertyByKeyID(uint32_t keyID, chaos::ArrayPointer<chaos::edb::KeyIdAndValue>& propertys);
        };
        
    }
    
}

#endif /* defined(__CHAOSFramework__Entity__) */

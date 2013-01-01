/*
 *	EntityDB.cpp
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
#include "EntityDB.h"
#include <chaos/common/data/entity/Entity.h>

using namespace chaos;
using namespace chaos::edb;

/*!
 Default constructor
 */
EntityDB::EntityDB() {
    entityInstanceSequence = 0;
}

/*!
 Default destructor
 */
EntityDB::~EntityDB() {
    for (std::map<atomic_int_type, entity::Entity*>::iterator iter = entityInstancesMap.begin();
         iter != entityInstancesMap.end(); iter++) {
        
        
        entity::Entity* entityToDelete = iter->second;
        delete entityToDelete;
    }
    entityInstancesMap.clear();
}

/*!
 Allocate and return a new instance of an Entity class
 */
entity::Entity* EntityDB::getNewEntityInstance(KeyIdAndValue& keyInfo) {
    atomic_int_type instance = atomic_increment(&entityInstanceSequence);
    entity::Entity *result = new entity::Entity(this, instance);

    if(result && !result->setEntityKeyAndInfo(keyInfo)) {
        //all is gone ok so i can add the isntance to the hash
        entityInstancesMap.insert(make_pair(instance, result));
    } else {
        //something is gone wrong, delete the instance
        if(result) {
            delete result;
            result = NULL;
        }
    }
    return result;
}

/*!
 Delete the isntance of an Entity class
 */
void EntityDB::deleteEntityInstance(entity::Entity* entity) {
    if(!entity) return;
    
    entityInstancesMap.erase(entity->instanceID);
    //delete the instance
    delete entity;
}
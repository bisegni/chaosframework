/*
 *	Entity.cpp
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

#include <chaos/common/data/entity/Entity.h>
#include <chaos/common/data/entity_db/EntityDB.h>

using namespace chaos;
using namespace chaos::entity;

Entity::Entity(edb::EntityDB *_database):Record(_database){

}

void Entity::setEntityKeyAndInfo(edb::EntityDB::KeyIdAndValue keyInfo) {
    //register the information for the entity
    database->getIDForEntity(keyInfo, entityID);
}

void Entity::addProperty(edb::EntityDB::KeyIdAndValue keyInfo) {
    int32_t idNewProperty = 0;
    if(!database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty)) {
        prepertyIDs.push_back(idNewProperty);
    }
}

void Entity::getAllProperty(chaos::ArrayPointer<chaos::edb::EntityDB::KeyIdAndValue>& propertys) {
    if(!database->searchPropertyForEntity(entityID, propertys)) {
        prepertyIDs.clear();
    }
}

void Entity::reset() {
    database->deleteAllPropertyForEntity(entityID);
}

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

Entity::Entity(edb::EntityDB *_database,  atomic_int_type _instanceID):Record(_database),instanceID(_instanceID){
}

Entity::~Entity(){
    
}

int32_t Entity::setEntityKeyAndInfo(chaos::edb::KeyIdAndValue& keyInfo) {
        //register the information for the entity
    return database->getIDForEntity(keyInfo, entityID);
}

int32_t Entity::addProperty(uint32_t keyID, string& propertyVal) {
    int32_t error = 0;
    uint32_t idNewProperty = 0;
    chaos::edb::KeyIdAndValue keyInfo;
    keyInfo.keyID = keyID;
    keyInfo.type = edb::KEY_STR_VALUE;
    strcpy(keyInfo.value.strValue, propertyVal.c_str())
    if(!(error = database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty))) {
        return error;
    }
    return error;
}

int32_t Entity::addProperty(uint32_t keyID, int64_t propertyVal) {
    int32_t error = 0;
    uint32_t idNewProperty = 0;
    chaos::edb::KeyIdAndValue keyInfo;
    keyInfo.keyID = keyID;
    keyInfo.type = edb::KEY_NUM_VALUE;
    keyInfo.value.numValue = propertyVal;
    if(!(error = database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty))) {
        return error;
    }
    return error;
}

int32_t Entity::addProperty(uint32_t keyID, double propertyVal) {
    int32_t error = 0;
    uint32_t idNewProperty = 0;
    chaos::edb::KeyIdAndValue keyInfo;
    keyInfo.keyID = keyID;
    keyInfo.type = edb::KEY_DOUBLE_VALUE;
    keyInfo.value.doubleValue = propertyVal;
    if(!(error = database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty))) {
        return error;
    }
    return error;

}

int32_t Entity::getAllProperty(chaos::ArrayPointer<chaos::edb::KeyIdAndValue>& propertys) {
    int32_t error = 0;
    if(!(error = database->searchPropertyForEntity(entityID, propertys))) {
        return error;
    }
    return error;
}

int32_t Entity::getPropertyByKeyID(uint32_t keyID, chaos::ArrayPointer<chaos::edb::KeyIdAndValue>& propertys) {
    int32_t error = 0;
    vector<uint32_t> keys = {keyID};
    if(!(error = database->searchPropertyForEntity(entityID, keys, propertys))) {
        return error;
    }
    return error;
}

int32_t Entity::reset() {
    return database->deleteAllPropertyForEntity(entityID);
}

int32_t Entity::addChild(Entity& entityChild) {
    return database->attachEntityChildToEntityParent(entityID, entityChild.entityID);
}

int32_t Entity::removeChild(Entity& entityChild) {
    return database->removeEntityChildFromEntityParent(entityID, entityChild.entityID);
}

int32_t Entity::removeAllChild() {
    return database->removeAllEntityChild(entityID);
}

int32_t Entity::getAllChild(vector<Entity*>& childs) {
    vector<uint32_t> childIDS;
    int32_t err = database->getAllChildEntity(entityID, childIDS);
    if(!err) {
        for (vector<uint32_t>::iterator iter = childIDS.begin();
             iter!= childIDS.end();
             iter++) {
            chaos::edb::KeyIdAndValue infoEntity;
            database->getEntityKeyInfoByID(*iter, infoEntity);
            childs.push_back(database->getNewEntityInstance(infoEntity));
        }
    }
    return err;
}

int32_t Entity::getChildsWithKeyID(uint32_t keyID, vector<Entity*>& childs) {
    vector<uint32_t> childIDS;
    int32_t err = database->searchEntityByParentIDAndKeyID(entityID, keyID, childIDS);
    if(!err) {
        for (vector<uint32_t>::iterator iter = childIDS.begin();
             iter!= childIDS.end();
             iter++) {
            chaos::edb::KeyIdAndValue infoEntity;
            database->getEntityKeyInfoByID(*iter, infoEntity);
            childs.push_back(database->getNewEntityInstance(infoEntity));
        }
    }
    return err;
}

int32_t Entity::getHasChildByKeyInfo(edb::KeyIdAndValue& keyInfo, bool& hasChild) {
    vector<uint32_t> childIDS;
    int32_t err = database->searchEntityByParentIDAndKeyValue(entityID, keyInfo, childIDS);
    hasChild = childIDS.size()>=1;
    return err;
}

int32_t Entity::getChildsWithKeyInfo(edb::KeyIdAndValue& keyInfo, vector<Entity*>& childs) {
    vector<uint32_t> childIDS;
    int32_t err = database->searchEntityByParentIDAndKeyValue(entityID, keyInfo, childIDS);
    if(!err) {
        for (vector<uint32_t>::iterator iter = childIDS.begin();
             iter!= childIDS.end();
             iter++) {
            chaos::edb::KeyIdAndValue infoEntity;
            database->getEntityKeyInfoByID(*iter, infoEntity);
            childs.push_back(database->getNewEntityInstance(infoEntity));
        }
    }
    return err;
}
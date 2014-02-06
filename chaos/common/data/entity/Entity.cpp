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
using namespace boost;
using namespace chaos;
using namespace chaos::entity;

#define KIV_MAKE(_n, _i, _t) chaos::edb::KeyIdAndValue _n;\
_n.keyID = _i;\
_n.type = _t;\

#define KIV_MAKE_STR(_n, _i, _t, _s) KIV_MAKE(_n, _i, _t)\
strcpy(_n.value.strValue, _s);

#define KIV_MAKE_NUM(_n, _i, _t, _num) KIV_MAKE(_n, _i, _t)\
_n.value.numValue = _num;

#define KIV_MAKE_DOUB(_n, _i, _t, _doub) KIV_MAKE(_n, _i, _t)\
_n.value.doubleValue = _doub;

Entity::Entity(edb::EntityDB *_database):Record(_database){
}

Entity::~Entity(){
    
}

int32_t Entity::setEntityKeyAndInfo(chaos::edb::KeyIdAndValue& keyInfo) {
        //register the information for the entity
    int32_t err = database->getIDForEntity(keyInfo, entityID);
    if(!err) {
        memcpy(&_keyValueInfo, &keyInfo, sizeof(chaos::edb::KeyIdAndValue));
    }
    return err;
}

int32_t Entity::addProperty(uint32_t keyID, const char * propertyVal) {
    uint32_t idNewProperty = 0;
    KIV_MAKE_STR(keyInfo, keyID, edb::KEY_STR_VALUE, propertyVal);
    return database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty);
}

int32_t Entity::updateProperty(uint32_t propertyID, uint32_t keyID, const char * propertyVal) {
    KIV_MAKE_STR(keyInfo, keyID, edb::KEY_STR_VALUE, propertyVal);
    return database->updatePropertyForEntity(propertyID, keyInfo);
}

int32_t Entity::addProperty(uint32_t keyID, string& propertyVal) {
    return addProperty(keyID, propertyVal.c_str());
}

int32_t Entity::updateProperty(uint32_t propertyID, uint32_t keyID, string& propertyVal) {
    return updateProperty(propertyID, keyID, propertyVal.c_str());
}

int32_t Entity::addProperty(uint32_t keyID, int64_t propertyVal) {
    uint32_t idNewProperty = 0;
    KIV_MAKE_NUM(keyInfo, keyID, edb::KEY_NUM_VALUE, propertyVal)
    return database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty);
}

int32_t Entity::updateProperty(uint32_t propertyID, uint32_t keyID, int64_t propertyVal) {
    KIV_MAKE_NUM(keyInfo, keyID, edb::KEY_NUM_VALUE, propertyVal)
    return database->updatePropertyForEntity(propertyID, keyInfo);
}

int32_t Entity::addProperty(uint32_t keyID, double propertyVal) {
    uint32_t idNewProperty = 0;
    KIV_MAKE_NUM(keyInfo, keyID, edb::KEY_DOUBLE_VALUE, propertyVal)
    return database->addNewPropertyForEntity(entityID, keyInfo, idNewProperty);
}

int32_t Entity::updateProperty(uint32_t propertyID, uint32_t keyID, double propertyVal) {
    KIV_MAKE_NUM(keyInfo, keyID, edb::KEY_DOUBLE_VALUE, propertyVal)
    return database->updatePropertyForEntity(propertyID, keyInfo);
}

int32_t Entity::getAllProperty(ptr_vector<chaos::edb::KeyIdAndValue>& propertys) {
    int32_t error = 0;
    if(!(error = database->searchPropertyForEntity(entityID, propertys))) {
        return error;
    }
    return error;
}

int32_t Entity::getPropertyByKeyID(uint32_t keyID, ptr_vector<chaos::edb::KeyIdAndValue>& propertys) {
    int32_t error = 0;
    vector<uint32_t> keys;
    keys.push_back(keyID);
    if(!(error = database->searchPropertyForEntity(entityID, keys, propertys))) {
        return error;
    }
    return error;
}


int32_t Entity::getPropertyByKeyID(vector<uint32_t>& keyIDs, ptr_vector<chaos::edb::KeyIdAndValue>& propertys) {
    int32_t error = 0;
    if(!(error = database->searchPropertyForEntity(entityID, keyIDs, propertys))) {
        return error;
    }
    return error;
}

int32_t Entity::deleteProrperty(uint32_t propertyID) {
	 return database->deleteProperty(propertyID);
}

int32_t Entity::resetAllProperty() {
    return database->deleteAllPropertyForEntity(entityID);
}

int32_t Entity::addChild(Entity& entityChild) {
    return database->attachEntityChildToEntityParent(entityID, entityChild.entityID);
}

int32_t Entity::isChild(Entity& entityChild, bool& isChild) {
    return database->checkParentChildJoined(entityID, entityChild.entityID, isChild);
}

int32_t Entity::removeChild(Entity& entityChild) {
    return database->removeEntityChildFromEntityParent(entityID, entityChild.entityID);
}

int32_t Entity::removeAllChild() {
    return database->removeAllEntityChild(entityID);
}

int32_t Entity::getAllChild(ptr_vector<Entity>& childs) {
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

int32_t Entity::getChildsWithKeyID(uint32_t keyID, ptr_vector<Entity>& childs) {
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

int32_t Entity::getChildWithPropertyKeyIDandValue(edb::KeyIdAndValue& keyInfo, ptr_vector<Entity>& childs) {
    vector<uint32_t> childIDS;
    int32_t err = database->searchEntityByParentAndPropertyIDAndKeyValue(entityID, keyInfo, childIDS);
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

int32_t Entity::getChildsWithKeyInfo(edb::KeyIdAndValue& keyInfo, ::ptr_vector<Entity>& childs) {
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
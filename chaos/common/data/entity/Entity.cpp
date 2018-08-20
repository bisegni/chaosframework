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

#include <chaos/common/data/entity/Entity.h>
#include <chaos/common/data/entity_db/EntityDB.h>
using namespace boost;
using namespace chaos;
using namespace chaos::entity;
using namespace std;
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

int32_t Entity::addProperty(uint32_t keyID, const string& propertyVal) {
	return addProperty(keyID, propertyVal.c_str());
}

int32_t Entity::updateProperty(uint32_t propertyID, uint32_t keyID, const string& propertyVal) {
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
	return database->searchPropertyForEntity(entityID, propertys);
}

int32_t Entity::getPropertyByKeyID(uint32_t keyID, ptr_vector<chaos::edb::KeyIdAndValue>& propertys) {
	vector<uint32_t> keys;
	keys.push_back(keyID);
	return database->searchPropertyForEntity(entityID, keys, propertys);
}


int32_t Entity::getPropertyByKeyID(vector<uint32_t>& keyIDs, ptr_vector<chaos::edb::KeyIdAndValue>& propertys) {
	return database->searchPropertyForEntity(entityID, keyIDs, propertys);
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
			//get the entity description
			entity::Entity *entity = database->getNewEntityInstance(infoEntity);
			assert(entity);
			
			//push it back in vector
			childs.push_back(entity);
		}
	}
	return err;
}

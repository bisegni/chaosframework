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

#include <string>
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/global.h>
#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>

#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace std;
using namespace boost;
using namespace chaos;
using namespace chaos::common::data;

#define MAKE_KEY(key, tmp) entityDB->getIDForKey(key, tmp);\
mapDatasetKeyForID.insert(make_pair(key, tmp));

#define CUSCHEMALDBG LDBG_ << "[CUSchemaDB] - "<<__PRETTY_FUNCTION__<<":"
#define CUSCHEMALERR LERR_ << "[CUSchemaDB] - ## Error "<<__PRETTY_FUNCTION__<<":"

void RangeValueInfo::reset() {
    defaultValue.clear();
    maxRange.clear();
    minRange.clear();
    maxSize = 0;
    valueType = (DataType::DataType)0;
    cardinality=0;
    dir=chaos::DataType::Undefined;
    binType.clear();
}

CUSchemaDB::CUSchemaDB() {
    initDB("CUSchemaDB", false);
}

/*!
 Parametrized constructor
 */
CUSchemaDB::CUSchemaDB(bool onMemory) {
    initDB("CUSchemaDB", onMemory);
}

/*!
 Parametrized constructor
 */
CUSchemaDB::CUSchemaDB(const char *databaseName, bool onMemory) {
    initDB(databaseName, onMemory);
}

void CUSchemaDB::initDB(const char *name, bool onMemory) {
    //create a database
    uint32_t keyTmp = 0;
    entityDB = utility::ObjectFactoryRegister<edb::EntityDB>::getInstance()->getNewInstanceByName("SQLiteEntityDB");
    if(!entityDB) return;
    std::string composeName(name);
    composeName.append(utility::UUIDUtil::generateUUIDLite());
    entityDB->initDB(composeName.c_str(), onMemory);
    
    MAKE_KEY(NodeDefinitionKey::NODE_UNIQUE_ID, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE, keyTmp);
    //new key for subbinary data
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING, keyTmp);
    MAKE_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_DESCRIPTION, keyTmp);
    
    MAKE_KEY(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST, keyTmp);
}

CUSchemaDB::~CUSchemaDB() {
    //remove all dataset
    for (EntityPtrMapIterator deviceEntityIter = deviceEntityMap.begin();
         deviceEntityIter != deviceEntityMap.end();
         deviceEntityIter++) {
        delete (deviceEntityIter->second);
    }
    
    //dispose internal database
    if(entityDB) {
        entityDB->deinitDB();
        delete(entityDB);
    }
}

/*
 Compose the attribute name
 */
void CUSchemaDB::composeAttributeName(const string& deviceID,
                                      const string& attributeName,
                                      string& composedName) {
    composedName.assign(deviceID).append(":").append(attributeName);
}

/*
 Compose the attribute name
 */
void CUSchemaDB::decomposeAttributeName(const string& deviceID,
                                        const string& attributeName,
                                        std::string& decomposed) {
    decomposed = attributeName.substr(deviceID.size()+1).c_str();
}

/*
 return the vector containing the atrtibute list for a domain
 */
entity::Entity* CUSchemaDB::getDeviceEntity(const string& deviceID) {
    if(deviceEntityMap.count(deviceID) > 0){
        return deviceEntityMap[deviceID];
    }
    //a new vector need to be added
    addDeviceId(deviceID);
    return deviceEntityMap[deviceID];
}

/*
 return the vector containing the atrtibute list for a domain
 */
entity::Entity* CUSchemaDB::getBinarySubtypeEntity(const string& node_uid,
                                                   const string& attribute_name) {
    //add subtype for binary data
    std::string a_name;
    edb::KeyIdAndValue sub_kiv;
    sub_kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE];
    sub_kiv.type = chaos::edb::KEY_STR_VALUE;
    a_name.clear();
    composeAttributeName(node_uid, boost::str(boost::format("%1%_%2%")%attribute_name%ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE), a_name);
    strcpy(sub_kiv.value.strValue, a_name.c_str());
    return entityDB->getNewEntityInstance(sub_kiv);
}
/*
 return the vector containing the atrtibute list for a domain
 */
void CUSchemaDB::addDeviceId(const string& deviceID) {
    if(deviceEntityMap.count(deviceID) > 0){
        return;
    }
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[NodeDefinitionKey::NODE_UNIQUE_ID];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    strcpy(kiv.value.strValue, deviceID.c_str());
    //add the entity for device
    entity::Entity *dsEntity = entityDB->getNewEntityInstance(kiv);
    if(dsEntity) {
        deviceEntityMap.insert(make_pair(deviceID, dsEntity));
        addUniqueAttributeProperty(dsEntity, mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP], (int64_t)utility::TimingUtil::getTimeStamp(), false);
    }
}

entity::Entity *CUSchemaDB::getDatasetElement(entity::Entity *device, string& attributeName) {
    return getDatasetElement(device, attributeName.c_str());
}

entity::Entity *CUSchemaDB::getDatasetElement(entity::Entity *device, const char * attributeName) {
    bool alreadyAdded = false;
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    composeAttributeName(device->getKeyInfo().value.strValue, attributeName, a_name);
    
    strncpy(kiv.value.strValue, a_name.c_str(), 255);
    
    ptr_vector<entity::Entity> childs;
    device->getHasChildByKeyInfo(kiv, alreadyAdded);
    if(alreadyAdded) {
        device->getChildsWithKeyInfo(kiv, childs);
        if(childs.size())
            return childs.release(childs.begin()).release();
        else
            return NULL;
    } else {
        entity::Entity *elementDst = entityDB->getNewEntityInstance(kiv);
        if(elementDst) {
            device->addChild(*elementDst);
        }
        return elementDst;
    }
}

/*
 Add the new field to the dataset
 */

void CUSchemaDB::addAttributeToDataSet(const std::string& node_uid,
                                       const std::string& attributeName,
                                       const std::string& attributeDescription,
                                       DataType::DataType attributeType,
                                       DataType::DataSetAttributeIOAttribute attributeDirection,
                                       uint32_t maxDimension) {
    
    uint32_t typeMaxDimension = 0;
    
    switch (attributeType) {
        case DataType::TYPE_BOOLEAN:
            typeMaxDimension = sizeof(bool);   //bson type
            break;
        case DataType::TYPE_DOUBLE:
            typeMaxDimension = sizeof(double);	//8 bytes (64-bit IEEE 754 floating point)
            break;
        case DataType::TYPE_INT32:
            typeMaxDimension = sizeof(int32_t);
            break;
        case DataType::TYPE_INT64:
            typeMaxDimension = sizeof(int64_t);
            break;
        case DataType::TYPE_CLUSTER:
        case DataType::TYPE_STRING:
            if(maxDimension == 0){
                CUSCHEMALDBG<<"WARNING: not json/string max length given setting to:"<<CUSCHEMA_DEFAULT_STRING_LENGHT;
                maxDimension = CUSCHEMA_DEFAULT_STRING_LENGHT;
            }
            
            typeMaxDimension = maxDimension;
            break;
        case DataType::TYPE_BYTEARRAY:
            if(maxDimension == 0)
                throw CException(1, "for byte array type need to be specified the max  string lenght", "CUSchemaDB::addAttributeToDataSet");
            typeMaxDimension = maxDimension;
            break;
        default:
            throw CException(2, "unmanaged type", "CUSchemaDB::addAttributeToDataSet");
            break;
    }
    
    bool isChild = false;
    entity::Entity *device = getDeviceEntity(node_uid);
    
    //add the attribute
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    
    composeAttributeName(node_uid, attributeName, a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    
    ChaosUniquePtr <entity::Entity> elementDst(entityDB->getNewEntityInstance(kiv));
    
    if(elementDst.get()) {
        addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION], attributeDescription);
        addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)attributeType);
        addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], (int64_t)attributeDirection);
        if(typeMaxDimension)addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE], (int64_t)typeMaxDimension);
        device->isChild(*elementDst.get(), isChild);
        if(!isChild) device->addChild(*elementDst);
    }
}

void CUSchemaDB::addBinaryAttributeAsSubtypeToDataSet(const std::string& node_uid,
                                                      const std::string& attribute_name,
                                                      const std::string& attribute_description,
                                                      DataType::BinarySubtype               subtype,
                                                      int32_t    cardinality,
                                                      DataType::DataSetAttributeIOAttribute attribute_direction) {
    bool isChild = false;
    entity::Entity *device = getDeviceEntity(node_uid);
    
    //add the attribute
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    
    composeAttributeName(node_uid, attribute_name, a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    
    ChaosUniquePtr<entity::Entity> element_dataset(entityDB->getNewEntityInstance(kiv));
    
    if(element_dataset.get()) {
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION], attribute_description);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], (int64_t)attribute_direction);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)DataType::TYPE_BYTEARRAY);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE], (int64_t)subtype);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY], (int64_t)cardinality);
        if(cardinality>0){addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE], (int64_t)cardinality);}
        
        device->isChild(*element_dataset.get(), isChild);
        if(!isChild) device->addChild(*element_dataset);
    }
}

void CUSchemaDB::addBinaryAttributeAsSubtypeToDataSet(const std::string& node_uid,
                                                      const std::string& attribute_name,
                                                      const std::string& attribute_description,
                                                      const std::vector<int32_t>&   subtype_list,
                                                      int32_t    cardinality,
                                                      DataType::DataSetAttributeIOAttribute attribute_direction) {
    bool isChild = false;
    entity::Entity *device = getDeviceEntity(node_uid);
    
    //add the attribute
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    
    composeAttributeName(node_uid, attribute_name, a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    
    ChaosUniquePtr<entity::Entity> element_dataset(entityDB->getNewEntityInstance(kiv));
    
    if(element_dataset.get()) {
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION], attribute_description);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], (int64_t)attribute_direction);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)DataType::TYPE_BYTEARRAY);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE], (int64_t)-1);//-1 indicate that we have an wntity taht collect all subtype
        if(cardinality>0){addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY], (int64_t)cardinality);}
        device->isChild(*element_dataset.get(), isChild);
        if(!isChild) device->addChild(*element_dataset);
        
        //add subtype for binary data
        edb::KeyIdAndValue sub_kiv;
        sub_kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE];
        sub_kiv.type = chaos::edb::KEY_STR_VALUE;
        a_name.clear();
        composeAttributeName(node_uid, boost::str(boost::format("%1%_%2%")%attribute_name%ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE), a_name);
        strcpy(sub_kiv.value.strValue, a_name.c_str());
        ChaosUniquePtr<entity::Entity> binary_subtype(entityDB->getNewEntityInstance(sub_kiv));
        if(binary_subtype.get()) {
            //we can add the list of the attribute
            for(std::vector<int>::const_iterator it = subtype_list.begin();
                it != subtype_list.end();
                it++) {
                //add the single attribute as normal type to subtype entity
                int32_t value = *it;
                binary_subtype->addProperty(mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)value);
            }
            //add subtype to dataset element entity
            element_dataset->isChild(*binary_subtype.get(), isChild);
            if(!isChild) element_dataset->addChild(*binary_subtype);
        }
    }
}

void CUSchemaDB::addBinaryAttributeAsMIMETypeToDataSet(const std::string& node_uid,
                                                       const std::string& attribute_name,
                                                       const std::string& attribute_description,
                                                       const std::string& mime_type,
                                                       DataType::DataSetAttributeIOAttribute attribute_direction) {
    bool isChild = false;
    entity::Entity *device = getDeviceEntity(node_uid);
    
    //add the attribute
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    
    composeAttributeName(node_uid, attribute_name, a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    
    ChaosUniquePtr<entity::Entity> element_dataset(entityDB->getNewEntityInstance(kiv));
    
    if(element_dataset.get()) {
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION], attribute_description);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)DataType::TYPE_BYTEARRAY);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], (int64_t)attribute_direction);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)DataType::TYPE_BYTEARRAY);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE], (int64_t)DataType::SUB_TYPE_MIME);
        addUniqueAttributeProperty(element_dataset.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_DESCRIPTION], mime_type);
        device->isChild(*element_dataset.get(), isChild);
        if(!isChild) device->addChild(*element_dataset);
    }
    
}

void CUSchemaDB::clearAllAttributeForProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd) {
    ptr_vector<edb::KeyIdAndValue> properties_for_entity;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, properties_for_entity);
    
    for (int idx = 0; idx < properties_for_entity.size(); idx++) {
        attributeEntity->deleteProrperty((&properties_for_entity[idx])->elementID);
    }
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, const char * attributeValue, bool checkValueForUnicity) {
    ptr_vector<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    int idx = 0;
    bool found = keysAndValues.size() != 0;
    
    if(found && checkValueForUnicity) {
        found = false;
        for (; idx< keysAndValues.size(); idx++) {
            if(!strcmp((&keysAndValues[idx])->value.strValue, attributeValue)) {
                found = true;
                break;
            } else {
                attributeEntity->deleteProrperty((&keysAndValues[idx])->elementID);
            }
        }
    }
    
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    }
    //else {
    //  attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
    //   }
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, const string& attributeValue, bool checkValueForUnicity) {
    ptr_vector<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    int idx = 0;
    bool found = keysAndValues.size() != 0;
    
    if(found && checkValueForUnicity) {
        found = false;
        for (; idx< keysAndValues.size(); idx++) {
            if(!strcmp((&keysAndValues[idx])->value.strValue, attributeValue.c_str())) {
                found = true;
                break;
            } else {
                attributeEntity->deleteProrperty((&keysAndValues[idx])->elementID);
            }
        }
    }
    
    //if(!found) attributeEntity->addProperty(keyIDToAdd, attributeValue);
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } /*else {
       attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
       }*/
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, int64_t attributeValue, bool checkValueForUnicity) {
    ptr_vector<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    int idx = 0;
    bool found = keysAndValues.size() != 0;
    
    if(found && checkValueForUnicity) {
        found = false;
        for (; idx< keysAndValues.size(); idx++) {
            if((&keysAndValues[idx])->value.numValue == attributeValue) {
                found = true;
                break;
            } else {
                attributeEntity->deleteProrperty((&keysAndValues[idx])->elementID);
            }
        }
    }
    
    //if(!found)  attributeEntity->addProperty(keyIDToAdd, attributeValue);
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } /*else {
       attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
       }*/
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, double attributeValue, bool checkValueForUnicity) {
    ptr_vector<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    int idx = 0;
    bool found = keysAndValues.size() != 0;
    
    if(found && checkValueForUnicity) {
        found = false;
        for (; idx< keysAndValues.size(); idx++) {
            if((&keysAndValues[idx])->value.doubleValue == attributeValue) {
                found = true;
                break;
            } else {
                attributeEntity->deleteProrperty((&keysAndValues[idx])->elementID);
            }
        }
    }
    
    //if(!found)  attributeEntity->addProperty(keyIDToAdd, attributeValue);
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } /*else {
       attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
       }*/
}
/*
 Add the new field at the CU dataset from the CDataWrapper
 */
void CUSchemaDB::addAttributeToDataSetFromDataWrapper(CDataWrapper& attributeDataWrapper) {
    //if(!attributeDataWrapper) return;
    
    string attributeDeviceID;
    string attributeName;
    string attributeDescription;
    
    if(!attributeDataWrapper.hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) return;
    attributeDeviceID = attributeDataWrapper.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    //get the entity for device
    entity::Entity *deviceEntity = getDeviceEntity(attributeDeviceID);
    
    if(attributeDataWrapper.hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)){
        
        ChaosUniquePtr<chaos::common::data::CDataWrapper> dataset_object(attributeDataWrapper.getCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION));
        
        if(dataset_object->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION)) {
            
            //get the entity for device
            entity::Entity *deviceEntity = getDeviceEntity(attributeDeviceID);
            CMultiTypeDataArrayWrapperSPtr elementsDescriptions = dataset_object->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
            
            for (int idx = 0; idx < elementsDescriptions->size(); idx++) {
                
                //next element in dataset
                CDWUniquePtr elementDescription=elementsDescriptions->getCDataWrapperElementAtIndex(idx);
                //attribute name
                
                if(!elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME))
                    continue;
                
                string attrName = elementDescription->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME);
                
                //get the attribute
                ChaosUniquePtr<entity::Entity> attributeEntity(getDatasetElement(deviceEntity, attrName));
                if(attributeEntity.get()==NULL){
                    // NOTE: enabling setAutoReconnection in deviceMessageChannel I get this null
                    continue;
                }
                //attribute description
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION], elementDescription->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION).c_str());
                }
                //attribute type
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE)) {
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE], (int64_t)elementDescription->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE));
                }
                //attribute direction
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], (int64_t)elementDescription->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION));
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE)) {
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE], elementDescription->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE).c_str());
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE], elementDescription->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE).c_str());
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE], elementDescription->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE).c_str());
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE], (int64_t)elementDescription->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE));
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE], (int64_t)elementDescription->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE));
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY], (int64_t)elementDescription->getInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY));
                }
                
                if(elementDescription->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING)){
                    addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING], elementDescription->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING));
                }
                
            }
        }
    }
    
    //add now the server address for this device if sent
    if(attributeDataWrapper.hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST)) {
        //remove all stored server
        clearAllAttributeForProperty(deviceEntity, mapDatasetKeyForID[DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST]);
        
        //in the package has been sent the address where fir the data for this device
        CMultiTypeDataArrayWrapperSPtr serverVec = attributeDataWrapper.getVectorValue(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
        for (int idx = 0; idx < serverVec->size(); idx++) {
            //add new server
            deviceEntity->addProperty(mapDatasetKeyForID[DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST],  serverVec->getStringElementAtIndex(idx).c_str());
        }
    }
}

/*
 fill a CDataWrapper with the dataset decode
 */
void CUSchemaDB::fillDataWrapperWithDataSetDescription(CDataWrapper& datasetDescription) {
    //now i must describe the param for this action
    // map<string, entity::Entity*> deviceEntityMap
    if(deviceEntityMap.size()){
        ChaosSharedPtr<CDataWrapper> datasetElementCDW;
        ChaosSharedPtr<CDataWrapper> domainDatasetDescription;
        
        //there are some parameter for this action, need to be added to rapresentation
        for (EntityPtrMapIterator deviceEntityIter = deviceEntityMap.begin();
             deviceEntityIter != deviceEntityMap.end();
             deviceEntityIter++) {
            //get domain name
            string deviceId = deviceEntityIter->first;
            entity::Entity *deviceEntity = deviceEntityIter->second;
            
            ChaosUniquePtr<chaos::common::data::CDataWrapper> domainDatasetDescription(new CDataWrapper());
            
            fillDataWrapperWithDataSetDescription(deviceEntity, *domainDatasetDescription.get());
            
            // add parametere representation object to main action representation
            datasetDescription.appendCDataWrapperToArray(*domainDatasetDescription.get());
        }
        
        //close array for all device description [IN CASE DATA FOR INITIALIZE THE DEVICE ONLY one description need to be returne SO THIS FUNCTION NEED TO BE CHANGED TO SUPPORT ALSO ONE ONLY DEVICE DS CREATION]
        datasetDescription.finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    }
}

/*!
 fill a CDataWrapper with the dataset decode
 */
void CUSchemaDB::fillDataWrapperWithDataSetDescription(string& deviceID, CDataWrapper& datasetDescription) {
    if(!deviceEntityMap.count(deviceID)) return;
    
    //get the netity for devceID
    entity::Entity *deviceEntity = deviceEntityMap[deviceID];
    
    fillDataWrapperWithDataSetDescription(deviceEntity, datasetDescription);
}

/*!
 fill a CDataWrapper with the dataset decode
 */
void CUSchemaDB::fillDataWrapperWithDataSetDescription(entity::Entity *deviceEntity, CDataWrapper& device_dataset_description) {
    ptr_vector<edb::KeyIdAndValue> attrProperty;
    ptr_vector<entity::Entity> device_dataset_attributes;
    
    //add deviceID to description data
    device_dataset_description.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, deviceEntity->getKeyInfo().value.strValue);
    
    //convenient array for element porperty
    CDataWrapper dataset;
    
    //set the registered timestamp
    deviceEntity->getPropertyByKeyID(mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP], attrProperty);
    if(attrProperty.size()) {
        dataset.addInt64Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_TIMESTAMP, (uint64_t)(&attrProperty[0])->value.numValue);
        attrProperty.release();
    }
    
    //try to get all dataset attribute for device entity
    deviceEntity->getChildsWithKeyID(mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME], device_dataset_attributes);
    
    //get dataset attribute for domain name
    
    
    for (ptr_vector<entity::Entity>::iterator dstElmtIterator = device_dataset_attributes.begin();
         dstElmtIterator != device_dataset_attributes.end();
         dstElmtIterator++) {
        
        //get next dst element entity for get the attribute
        entity::Entity *dstAttrEntity = &(*dstElmtIterator);
        
        //get all entity property
        attrProperty.release();
        dstAttrEntity->getAllProperty(attrProperty);
        if(attrProperty.size() == 0) continue;
        
        //cicle all dataset element
        ChaosUniquePtr<chaos::common::data::CDataWrapper> dataset_element(new CDataWrapper());
        
        fillCDataWrapperDSAtribute(dataset_element.get(), deviceEntity, dstAttrEntity, attrProperty);
        
        // add parameter representation object to main action representation
        dataset.appendCDataWrapperToArray(*dataset_element.get());
    }
    
    //close the dataset attribute array
    dataset.finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION);
    device_dataset_description.addCSDataValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DESCRIPTION, dataset);
    
    //add now the server address for this device if sent
    attrProperty.clear();
    deviceEntity->getPropertyByKeyID(mapDatasetKeyForID[DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST], attrProperty);
    for (ptr_vector<edb::KeyIdAndValue>::iterator iter = attrProperty.begin();
         iter != attrProperty.end();
         iter++) {
        edb::KeyIdAndValue *kivPtr = &(*iter);
        device_dataset_description.appendStringToArray(kivPtr->value.strValue);
    }
    device_dataset_description.finalizeArrayForKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
}

/*
 */
void CUSchemaDB::fillCDataWrapperDSAtribute(CDataWrapper *dsAttribute,
                                            entity::Entity *deviceIDEntity,
                                            entity::Entity *attrEntity,
                                            ptr_vector<edb::KeyIdAndValue>& attrProperty) {
    
    //add name
    edb::KeyIdAndValue *curKIV = NULL;
    string dID = deviceIDEntity->getKeyInfo().value.strValue;
    string attr = attrEntity->getKeyInfo().value.strValue;
    string attrDecomposed = attr.substr(dID.size()+1);
    
    dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME, attrDecomposed);
    
    for (ptr_vector<edb::KeyIdAndValue>::iterator dstElmtIterator = attrProperty.begin();
         dstElmtIterator != attrProperty.end();
         dstElmtIterator++) {
        //cicle the property
        curKIV = &(*dstElmtIterator);
        if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION]) {
            dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION]) {
            dsAttribute->addInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION, (int32_t)curKIV->value.numValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE]) {
            dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE]) {
            dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE]){
            dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE]){
            dsAttribute->addInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE, (int32_t)curKIV->value.numValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE]) {
            dsAttribute->addInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE, (int32_t)curKIV->value.numValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE]) {
            //custom attribute
            if(curKIV->value.numValue == -1) {
                //we have subtype collected in a separate entity so in this case the attribute value is an array
                ChaosUniquePtr<entity::Entity> sub_tyhpe_entity(getBinarySubtypeEntity(dID, attrDecomposed));
                if(sub_tyhpe_entity.get()) {
                    ptr_vector<edb::KeyIdAndValue> sub_types_list;
                    //we need to get all rpoperty
                    sub_tyhpe_entity->getAllProperty(sub_types_list);
                    if(sub_types_list.size()) {
                        for(ptr_vector<edb::KeyIdAndValue>::iterator it = sub_types_list.begin();
                            it != sub_types_list.end();
                            it++) {
                            //no we need to add numerica value as 32 bit into the array for attribute subtype
                            dsAttribute->appendInt32ToArray((int32_t)it->value.numValue);
                        }
                        dsAttribute->finalizeArrayForKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE);
                    }
                }
            } else {
                //we have a mime type
                dsAttribute->addInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE, (int32_t)curKIV->value.numValue);
            }
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY]){
            dsAttribute->addInt32Value(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY, (int32_t)curKIV->value.numValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING]){
            dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_ENCODING, curKIV->value.strValue);
        }else if(curKIV->keyID == mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_DESCRIPTION]){
            dsAttribute->addStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_MIME_DESCRIPTION, curKIV->value.strValue);
        }
    }
}

/*
 return al domain
 */
void CUSchemaDB::getAllDeviceId(vector<string>& deviceNames) {
    for (EntityPtrMapIterator deviceEntityIter = deviceEntityMap.begin();
         deviceEntityIter != deviceEntityMap.end();
         deviceEntityIter++) {
        
        //add domain name
        deviceNames.push_back(deviceEntityIter->first);
    }
}

/*
 return al domain
 */
bool CUSchemaDB::deviceIsPresent(const string& deviceID) {
    return deviceEntityMap.count(deviceID)==1;
}

void CUSchemaDB::getDeviceDatasetAttributesName(const string& deviceID,
                                                vector<string>& attributesName) {
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    ptr_vector<entity::Entity> attrDst;
    deviceEntity->getChildsWithKeyID(mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], attrDst);
    for (ptr_vector<entity::Entity>::iterator attrEntityIter = attrDst.begin();
         attrEntityIter != attrDst.end();
         attrEntityIter++) {
        entity::Entity *e = &(*attrEntityIter);
        attributesName.push_back(e->getKeyInfo().value.strValue);
    }
}

void CUSchemaDB::getDeviceDatasetAttributesName(const string& deviceID,
                                                DataType::DataSetAttributeIOAttribute directionType,
                                                vector<string>& attributesName) {
    //get the device entity
    ptr_vector<entity::Entity> entityVec;
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION];
    kiv.type = edb::KEY_NUM_VALUE;
    kiv.value.numValue = static_cast<int64_t>(directionType);
    
    deviceEntity->getChildWithPropertyKeyIDandValue(kiv, entityVec);
    //create all attribute datawrapper for all entity
    ptr_vector<edb::KeyIdAndValue> attrProperty;
    for (ptr_vector<entity::Entity>::iterator dstElmtIterator = entityVec.begin();
         dstElmtIterator != entityVec.end();
         dstElmtIterator++) {
        
        entity::Entity *e = &(*dstElmtIterator);
        string attrName = e->getKeyInfo().value.strValue;
        attributesName.push_back(attrName.substr(deviceID.length()+1));
    }
}

void CUSchemaDB::getDeviceAttributeDescription(const string& deviceID,
                                               const string& attributesName,
                                               string& attributeDescription) {
    ptr_vector<edb::KeyIdAndValue> attrPropertyVec;
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    
    ChaosUniquePtr<entity::Entity> attributeDstEntity(getDatasetElement(deviceEntity, attributesName.c_str()));
    if(!attributeDstEntity.get()) return;
    
    attributeDstEntity->getPropertyByKeyID(mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DESCRIPTION], attrPropertyVec);
    if(!attrPropertyVec.size()) return;
    
    attributeDescription = (&attrPropertyVec[0])->value.strValue;
}

int CUSchemaDB::getDeviceAttributeRangeValueInfo(const string& deviceID,
                                                 const string& attributesName,
                                                 RangeValueInfo& rangeInfo) {
    
    string a_name;
    edb::KeyIdAndValue kiv;
    vector<uint32_t> keyToGot;
    ptr_vector<entity::Entity> attrEntityVec;
    ptr_vector<edb::KeyIdAndValue> attrPropertyVec;
    rangeInfo.reset();
    CUSCHEMALDBG<<" DEVICE:"<<deviceID<<" attr:"<<attributesName;
    //clear all information
    kiv.keyID = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_NAME];
    kiv.type = edb::KEY_STR_VALUE;
    
    composeAttributeName(deviceID.c_str(), attributesName.c_str(), a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    
    //get all
    deviceEntity->getChildsWithKeyInfo(kiv, attrEntityVec);
    
    //check if we ha found the attribute
    if(!attrEntityVec.size()) return 1;
    
    
    uint32_t keyIdAttrMaxRng = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE];
    uint32_t keyIdAttrMinRng = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE];
    uint32_t keyIdAttrDefaultValue = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE];
    uint32_t keyIdAttrType = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_TYPE];
    uint32_t keyIdAttrMaxSize = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_VALUE_MAX_SIZE];
    uint32_t keyIdAttrBinType =mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_SUBTYPE];
    uint32_t keyIdAttrCardinality =mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_BINARY_CARDINALITY];
    uint32_t keyIdAttrDir =mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION];
    
    keyToGot.push_back(keyIdAttrMaxRng);
    keyToGot.push_back(keyIdAttrMinRng);
    keyToGot.push_back(keyIdAttrDefaultValue);
    keyToGot.push_back(keyIdAttrType);
    keyToGot.push_back(keyIdAttrMaxSize);
    keyToGot.push_back(keyIdAttrBinType);
    keyToGot.push_back(keyIdAttrCardinality);
    keyToGot.push_back(keyIdAttrDir);
    
    (&attrEntityVec[0])->getPropertyByKeyID(keyToGot, attrPropertyVec);
    if(!attrPropertyVec.size()) return 1;
    rangeInfo.name=attributesName;
    for (ptr_vector<edb::KeyIdAndValue>::iterator attrPropertyIterator = attrPropertyVec.begin();
         attrPropertyIterator != attrPropertyVec.end();
         attrPropertyIterator++) {
        
        edb::KeyIdAndValue *kivPtr = &(*attrPropertyIterator);
        
        if(kivPtr->keyID == keyIdAttrMaxSize) {
            rangeInfo.maxSize = (uint32_t)kivPtr->value.numValue;
        } else if(kivPtr->keyID == keyIdAttrMaxRng) {
            rangeInfo.maxRange = kivPtr->value.strValue;
        } else if(kivPtr->keyID == keyIdAttrMinRng) {
            rangeInfo.minRange = kivPtr->value.strValue;
        } else if(kivPtr->keyID == keyIdAttrType) {
            rangeInfo.valueType = (DataType::DataType)kivPtr->value.numValue;
        } else if(kivPtr->keyID == keyIdAttrBinType){
            rangeInfo.binType.clear();
            //rangeInfo.binType = (DataType::BinarySubtype)kivPtr->value.numValue;
            //custom attribute
            if(kivPtr->value.numValue == -1) {
                //we have subtype collected in a separate entity so in this case the attribute value is an array
                ChaosUniquePtr<entity::Entity> sub_tyhpe_entity(getBinarySubtypeEntity(deviceID, attributesName));
                if(sub_tyhpe_entity.get()) {
                    ptr_vector<edb::KeyIdAndValue> sub_types_list;
                    //we need to get all rpoperty
                    sub_tyhpe_entity->getAllProperty(sub_types_list);
                    if(sub_types_list.size()) {
                        for(ptr_vector<edb::KeyIdAndValue>::iterator it = sub_types_list.begin();
                            it != sub_types_list.end();
                            it++) {
                            //no we need to add numerica value as 32 bit into the array for attribute subtype
                            rangeInfo.binType.push_back(static_cast<DataType::BinarySubtype>(it->value.numValue));
                        }
                    }
                }
            } else {
                rangeInfo.binType.push_back(static_cast<DataType::BinarySubtype>(kivPtr->value.numValue));
            }
        } else if(kivPtr->keyID == keyIdAttrCardinality){
            rangeInfo.cardinality = (uint32_t)kivPtr->value.numValue;
        } else if(kivPtr->keyID == keyIdAttrDir){
            rangeInfo.dir = (DataType::DataSetAttributeIOAttribute)kivPtr->value.numValue;
        } else if(kivPtr->keyID == keyIdAttrDefaultValue) {
            switch (kivPtr->type) {
                case edb::KEY_DOUBLE_VALUE:
                    rangeInfo.defaultValue = lexical_cast<string>(kivPtr->value.doubleValue);
                    break;
                case edb::KEY_NUM_VALUE:
                    rangeInfo.defaultValue = lexical_cast<string>(kivPtr->value.numValue);
                    break;
                case edb::KEY_STR_VALUE:
                    rangeInfo.defaultValue = kivPtr->value.strValue;
                    break;
            }
            
        }
    }
    
    
    return 0;
}

//!Set the range values for an attribute
/*!
 set the range value for the attribute
 \param deviceID the identification of the device
 \param attributesName the name of the attribute
 \param rangeInfo the range and default value of the attribute, the fields
 of the struct are not cleaned, so if an attrbute doesn't has
 some finromation, relative field are not touched.
 */
void CUSchemaDB::setDeviceAttributeRangeValueInfo(const string& deviceID,
                                                  const string& attributesName,
                                                  RangeValueInfo& rangeInfo) {
    
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    
    //get the attribute
    ChaosUniquePtr<entity::Entity> attributeEntity(getDatasetElement(deviceEntity, attributesName.c_str()));
    
    uint32_t keyIdAttrMaxRng = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MAX_RANGE];
    uint32_t keyIdAttrMinRng = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_MIN_RANGE];
    uint32_t keyIdAttrDefaultValue = mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_DEFAULT_VALUE];
    
    addUniqueAttributeProperty(attributeEntity.get(), keyIdAttrMaxRng, rangeInfo.maxRange);
    addUniqueAttributeProperty(attributeEntity.get(), keyIdAttrMinRng, rangeInfo.minRange);
    addUniqueAttributeProperty(attributeEntity.get(), keyIdAttrDefaultValue, rangeInfo.defaultValue);
}

int CUSchemaDB::getDeviceAttributeDirection(const string& deviceID,
                                            const string& attributesName,
                                            DataType::DataSetAttributeIOAttribute& directionType) {
    
    ptr_vector<edb::KeyIdAndValue> attrPropertyVec;
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    //check if we ha found the attribute
    if(!deviceEntity) return 1;
    
    ChaosUniquePtr<entity::Entity> attributeDstEntity(getDatasetElement(deviceEntity, attributesName.c_str()));
    
    attributeDstEntity->getPropertyByKeyID(mapDatasetKeyForID[ControlUnitNodeDefinitionKey::CONTROL_UNIT_DATASET_ATTRIBUTE_DIRECTION], attrPropertyVec);
    if(!attrPropertyVec.size()) return 1;
    
    directionType = static_cast<DataType::DataSetAttributeIOAttribute>((&attrPropertyVec[0])->value.numValue);
    return 0;
}

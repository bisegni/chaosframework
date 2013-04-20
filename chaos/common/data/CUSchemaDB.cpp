/*
 *	CUSchemaDB.cpp
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

#include <string>
#include <boost/lexical_cast.hpp>

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/global.h>
#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>
using namespace chaos;
using namespace std;
using namespace boost;


#define MAKE_KEY(key, tmp) entityDB->getIDForKey(key, tmp);\
                           mapDatasetKeyForID.insert(make_pair<const char *, uint32_t>(key, tmp));

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
    entityDB = ObjectFactoryRegister<edb::EntityDB>::getInstance()->getNewInstanceByName("SQLiteEntityDB");
    if(!entityDB) return;
    std::string composeName(name);
    composeName.append(UUIDUtil::generateUUIDLite());
    entityDB->initDB(composeName.c_str(), onMemory);
    
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_TIMESTAMP, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE, keyTmp);
    MAKE_KEY(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE, keyTmp);
    MAKE_KEY(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS, keyTmp);
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
void CUSchemaDB::composeAttributeName(const char *deviceID, const char *attributeName, string& composedName) {
    composedName.assign(deviceID).append(":").append(attributeName);
} 

/*
 Compose the attribute name
*/ 
const char * CUSchemaDB::decomposeAttributeName(string& deviceID, string& attributeName) {
    return attributeName.substr(deviceID.size()+1).c_str();
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
void CUSchemaDB::addDeviceId(const string& deviceID) {
    if(deviceEntityMap.count(deviceID) > 0){
        return;
    }
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    strcpy(kiv.value.strValue, deviceID.c_str());
        //add the entity for device
    entity::Entity *dsEntity = entityDB->getNewEntityInstance(kiv);
    if(dsEntity) {
        deviceEntityMap.insert(make_pair<string, entity::Entity*>(deviceID, dsEntity));
        addUniqueAttributeProperty(dsEntity, mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_TIMESTAMP], timingUtils.getTimeStamp(), false);
    }
}

entity::Entity *CUSchemaDB::getDatasetElement(entity::Entity *device, string& attributeName) {
    return getDatasetElement(device, attributeName.c_str());
}

entity::Entity *CUSchemaDB::getDatasetElement(entity::Entity *device, const char * attributeName) {
    bool alreadyAdded = false;
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    composeAttributeName(device->getKeyInfo().value.strValue, attributeName, a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    
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
void CUSchemaDB::addAttributeToDataSet(const char*const attributeDeviceID,
                                       const char*const attributeName,
                                       const char*const attributeDescription,
                                       DataType::DataType attributeType,
                                       DataType::DataSetAttributeIOAttribute attributeDirection) {
    
    
    bool isChild = false;
    entity::Entity *device = getDeviceEntity(attributeDeviceID);
    
        //add the attribute
    string a_name;
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME];
    kiv.type = chaos::edb::KEY_STR_VALUE;
    
    composeAttributeName(attributeDeviceID, attributeName, a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    
    auto_ptr<entity::Entity> elementDst(entityDB->getNewEntityInstance(kiv));
    
    if(elementDst.get()) {
        addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION], attributeDescription);
        addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE], (int64_t)attributeType);
        addUniqueAttributeProperty(elementDst.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION], (int64_t)attributeDirection);
        device->isChild(*elementDst.get(), isChild);
        if(!isChild) device->addChild(*elementDst);
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
            }
        }
    }
    
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } else {
        attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
    }
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, string& attributeValue, bool checkValueForUnicity) {
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
            }
        }
    }
    
    //if(!found) attributeEntity->addProperty(keyIDToAdd, attributeValue);
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } else {
        attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
    }
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
            }
        }
    }
    
    //if(!found)  attributeEntity->addProperty(keyIDToAdd, attributeValue);
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } else {
        attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
    }
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
            }
        }
    }
    
    //if(!found)  attributeEntity->addProperty(keyIDToAdd, attributeValue);
    if(!found) {
        attributeEntity->addProperty(keyIDToAdd, attributeValue);
    } else {
        attributeEntity->updateProperty((&keysAndValues[idx])->elementID, keyIDToAdd, attributeValue);
    }
}
/*
 Add the new field at the CU dataset from the CDataWrapper
 */
void CUSchemaDB::addAttributeToDataSetFromDataWrapper(CDataWrapper& attributeDataWrapper) {
        //if(!attributeDataWrapper) return;
    
    string attributeDeviceID;
    string attributeName;
    string attributeDescription;
    auto_ptr<CDataWrapper> elementDescription;
    auto_ptr<CMultiTypeDataArrayWrapper> elementsDescriptions;
    
    if(!attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)) return;
    attributeDeviceID = attributeDataWrapper.getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    //get the entity for device
    entity::Entity *deviceEntity = getDeviceEntity(attributeDeviceID);
    
    if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)){
            //get the entity for device
        entity::Entity *deviceEntity = getDeviceEntity(attributeDeviceID);
        
        elementsDescriptions.reset(attributeDataWrapper.getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
        
        for (int idx = 0; idx < elementsDescriptions->size(); idx++) {
            
            
                //next element in dataset
            elementDescription.reset(elementsDescriptions->getCDataWrapperElementAtIndex(idx));
                //attribute name
            
            if(!elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME))
                continue;
            
            
            string attrName = elementDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME);
            
                //get the attribute
            auto_ptr<entity::Entity> attributeEntity(getDatasetElement(deviceEntity, attrName));
            
            
                //attribute description
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION)){
                addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION], elementDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION).c_str());
            }
                //attribute type
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE)) {
                addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE], (int64_t)elementDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE));
            }
                //attribute direction
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION)){
                addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION], (int64_t)elementDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION));
            }
            
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE)) {
                addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE], elementDescription->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE).c_str());
            }
            
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE)){
                addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE], elementDescription->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE).c_str());
            }
            
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE)){
                addUniqueAttributeProperty(attributeEntity.get(), mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE], elementDescription->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE).c_str());
            }
        }
     }
    
    //add now the server address for this device if sent
    if(attributeDataWrapper.hasKey(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS)) {
        //in the package has been sent the address where fir the data for this device
        auto_ptr<CMultiTypeDataArrayWrapper> serverVec(attributeDataWrapper.getVectorValue(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS));
        for (int idx = 0; idx < serverVec->size(); idx++) {
            addUniqueAttributeProperty(deviceEntity, mapDatasetKeyForID[LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS], serverVec->getStringElementAtIndex(idx).c_str());
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
        shared_ptr<CDataWrapper> datasetElementCDW;
        shared_ptr<CDataWrapper> domainDatasetDescription;
        
            //there are some parameter for this action, need to be added to rapresentation
        for (EntityPtrMapIterator deviceEntityIter = deviceEntityMap.begin();
             deviceEntityIter != deviceEntityMap.end();
             deviceEntityIter++) {
                //get domain name
            string deviceId = deviceEntityIter->first;
            entity::Entity *deviceEntity = deviceEntityIter->second;

            auto_ptr<CDataWrapper> domainDatasetDescription(new CDataWrapper());
            
            fillDataWrapperWithDataSetDescription(deviceEntity, *domainDatasetDescription.get());
            
                // add parametere representation object to main action representation
            datasetDescription.appendCDataWrapperToArray(*domainDatasetDescription.get());
        }
        
        //close array for all device description [IN CASE DATA FOR INITIALIZE THE DEVICE ONLY one description need to be returne SO THIS FUNCTION NEED TO BE CHANGED TO SUPPORT ALSO ONE ONLY DEVICE DS CREATION]
        datasetDescription.finalizeArrayForKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION);
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
void CUSchemaDB::fillDataWrapperWithDataSetDescription(entity::Entity *deviceEntity, CDataWrapper& deviceDatasetDescription) {
    ptr_vector<edb::KeyIdAndValue> attrProperty;
    ptr_vector<entity::Entity> deviceDatasetAttribute;
    
    //add deviceID to description data
    deviceDatasetDescription.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, deviceEntity->getKeyInfo().value.strValue);
    
    //try to get all dataset attribute for device entity
    deviceEntity->getChildsWithKeyID(mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME], deviceDatasetAttribute);
    
    //get dataset attribute for domain name
    
    //convenient array for element porperty
    
    for (ptr_vector<entity::Entity>::iterator dstElmtIterator = deviceDatasetAttribute.begin();
         dstElmtIterator != deviceDatasetAttribute.end();
         dstElmtIterator++) {
        
        //get next dst element entity for get the attribute
        entity::Entity *dstAttrEntity = &(*dstElmtIterator);
        
        //get all entity property
        attrProperty.release();
        dstAttrEntity->getAllProperty(attrProperty);
        if(attrProperty.size() == 0) continue;
        
        //cicle all dataset element
        auto_ptr<CDataWrapper> datasetElementCDW(new CDataWrapper());
        
        fillCDataWrapperDSAtribute(datasetElementCDW.get(), deviceEntity, dstAttrEntity, attrProperty);
        
        // add parametere representation object to main action representation
        deviceDatasetDescription.appendCDataWrapperToArray(*datasetElementCDW.get());
        //CDataWrapper *data = *datasetIterator;
    }
    
    //close the dataset attribute array
    deviceDatasetDescription.finalizeArrayForKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION);
    
    //add now the server address for this device if sent
    attrProperty.clear();
    deviceEntity->getPropertyByKeyID(mapDatasetKeyForID[LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS], attrProperty);
    for (ptr_vector<edb::KeyIdAndValue>::iterator iter = attrProperty.begin();
         iter != attrProperty.end();
         iter++) {
        edb::KeyIdAndValue *kivPtr = &(*iter);
        deviceDatasetDescription.appendStringToArray(kivPtr->value.strValue);
    }
    deviceDatasetDescription.finalizeArrayForKey(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS);
}

/*
 */
void CUSchemaDB::fillCDataWrapperDSAtribute(CDataWrapper *dsAttribute, entity::Entity *deviceIDEntity, entity::Entity *attrEntity, ptr_vector<edb::KeyIdAndValue>& attrProperty) {
    
    //add name
    string dID = deviceIDEntity->getKeyInfo().value.strValue;
    string attr = attrEntity->getKeyInfo().value.strValue;
    string attrDecomposed = attr.substr(dID.size()+1);
    
    dsAttribute->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, attrDecomposed);
    
    for (ptr_vector<edb::KeyIdAndValue>::iterator dstElmtIterator = attrProperty.begin();
         dstElmtIterator != attrProperty.end();
         dstElmtIterator++) {
        //cicle the property
        edb::KeyIdAndValue *curKIV = &(*dstElmtIterator);
        if(curKIV->keyID == mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION]) {
            dsAttribute->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION]) {
            dsAttribute->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION, (int32_t)curKIV->value.numValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE]) {
            dsAttribute->addStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE]) {
            dsAttribute->addStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE]){
            dsAttribute->addStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE, curKIV->value.strValue);
        } else if(curKIV->keyID == mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE]) {
            dsAttribute->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE, (int32_t)curKIV->value.numValue);
        } else {
            //custom attribute
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

void CUSchemaDB::getDeviceDatasetAttributesName(const string& deviceID, vector<string>& attributesName) {
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    ptr_vector<entity::Entity> attrDst;
    deviceEntity->getChildsWithKeyID(mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION], attrDst);
    for (ptr_vector<entity::Entity>::iterator attrEntityIter = attrDst.begin();
         attrEntityIter != attrDst.end();
         attrEntityIter++) {
        entity::Entity *e = &(*attrEntityIter);
        attributesName.push_back(e->getKeyInfo().value.strValue);
    }
}

void CUSchemaDB::getDeviceDatasetAttributesName(const string& deviceID,  DataType::DataSetAttributeIOAttribute directionType, vector<string>& attributesName) {
    //get the device entity
    ptr_vector<entity::Entity> entityVec;
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    
    edb::KeyIdAndValue kiv;
    kiv.keyID = mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION];
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

void CUSchemaDB::getDeviceAttributeDescription(const string& deviceID, const string& attributesName, string& attributeDescription) {
    ptr_vector<edb::KeyIdAndValue> attrPropertyVec;
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);

    auto_ptr<entity::Entity> attributeDstEntity(getDatasetElement(deviceEntity, attributesName.c_str()));
    if(!attributeDstEntity.get()) return;
    
    attributeDstEntity->getPropertyByKeyID(mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION], attrPropertyVec);
    if(!attrPropertyVec.size()) return;
    
    attributeDescription = (&attrPropertyVec[0])->value.strValue;
}

void CUSchemaDB::getDeviceAttributeRangeValueInfo(const string& deviceID, const string& attributesName, RangeValueInfo& rangeInfo) {
    
    string a_name;
    edb::KeyIdAndValue kiv;
    vector<uint32_t> keyToGot;
    ptr_vector<entity::Entity> attrEntityVec;
    ptr_vector<edb::KeyIdAndValue> attrPropertyVec;

    kiv.keyID = mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME];
    kiv.type = edb::KEY_STR_VALUE;
    
    composeAttributeName(deviceID.c_str(), attributesName.c_str(), a_name);
    
    strcpy(kiv.value.strValue, a_name.c_str());
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    
    //get all
    deviceEntity->getChildsWithKeyInfo(kiv, attrEntityVec);
    
    //check if we ha found the attribute
    if(!attrEntityVec.size()) return;
    
    
    uint32_t keyIdAttrMaxRng = mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE];
    uint32_t keyIdAttrMinRng = mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE];
    uint32_t keyIdAttrDefaultValue = mapDatasetKeyForID[DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE];
    uint32_t keyIdAttrType = mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE];
    
    keyToGot.push_back(keyIdAttrMaxRng);
    keyToGot.push_back(keyIdAttrMinRng);
    keyToGot.push_back(keyIdAttrDefaultValue);
    keyToGot.push_back(keyIdAttrType);
    
    
    (&attrEntityVec[0])->getPropertyByKeyID(keyToGot, attrPropertyVec);
    if(!attrPropertyVec.size()) return;
    
    for (ptr_vector<edb::KeyIdAndValue>::iterator attrPropertyIterator = attrPropertyVec.begin();
         attrPropertyIterator != attrPropertyVec.end();
         attrPropertyIterator++) {
        
        edb::KeyIdAndValue *kivPtr = &(*attrPropertyIterator);
        
        if(kivPtr->keyID == keyIdAttrMaxRng) {
            rangeInfo.maxRange = kivPtr->value.strValue;
        } else if(kivPtr->keyID == keyIdAttrMinRng) {
            rangeInfo.minRange = kivPtr->value.strValue;
        } else if(kivPtr->keyID == keyIdAttrType) {
            rangeInfo.valueType = (DataType::DataType)kivPtr->value.numValue;
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
}

int CUSchemaDB::getDeviceAttributeDirection(const string& deviceID, const string& attributesName, DataType::DataSetAttributeIOAttribute& directionType) {

    ptr_vector<edb::KeyIdAndValue> attrPropertyVec;
    entity::Entity *deviceEntity = getDeviceEntity(deviceID);
    //check if we ha found the attribute
    if(!deviceEntity) return 1;
        
    auto_ptr<entity::Entity> attributeDstEntity(getDatasetElement(deviceEntity, attributesName.c_str()));
    
    attributeDstEntity->getPropertyByKeyID(mapDatasetKeyForID[DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION], attrPropertyVec);
    if(!attrPropertyVec.size()) return 1;
    
    directionType = static_cast<DataType::DataSetAttributeIOAttribute>((&attrPropertyVec[0])->value.numValue);
    return 0;
}

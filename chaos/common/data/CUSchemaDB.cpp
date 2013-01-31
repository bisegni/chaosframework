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

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/global.h>
#include <chaos/common/data/CUSchemaDB.h>
#include <chaos/common/data/CDataWrapper.h>
using namespace chaos;
using namespace std;
using namespace boost;

CUSchemaDB::CUSchemaDB() {
        //create a database
    entityDB = ObjectFactoryRegister<edb::EntityDB>::getInstance()->getNewInstanceByName("SQLiteEntityDB");
    if(!entityDB) return;
    
    entityDB->initDB("CUSchema");
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, keyIdDevice);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION, keyIdDataset);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, keyIdAttrName);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION, keyIdAttrDesc);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE, keyIdAttrType);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION, keyIdAttrDir);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE, keyIdAttrMaxRng);
    entityDB->getIDForKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE, keyIdAttrMinRng);
}

CUSchemaDB::~CUSchemaDB() {
        //remove all dataset
    clearAllDatasetsVectors();
    
        //dispose internal database
    if(entityDB) {
        entityDB->deinitDB();
        delete(entityDB);
    }
}

void CUSchemaDB::clearAllDatasetsVectors() {
    
}

void CUSchemaDB::clearDatasetForDeviceID(string& deviceID){
    
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
void CUSchemaDB::addDeviceId(string deviceID) {
    if(deviceEntityMap.count(deviceID) > 0){
        return;
    }
    edb::KeyIdAndValue kiv;
    kiv.keyID = keyIdDevice;
    kiv.type = chaos::edb::KEY_STR_VALUE;
    strcpy(kiv.value.strValue, deviceID.c_str());
        //add the entity for device
    deviceEntityMap.insert(make_pair<string, entity::Entity*>(deviceID, entityDB->getNewEntityInstance(kiv)));
}

entity::Entity *CUSchemaDB::getDatasetElement(entity::Entity *device, string& attributeName) {
    bool alreadyAdded = false;
    edb::KeyIdAndValue kiv;
    kiv.keyID = keyIdDataset;
    kiv.type = chaos::edb::KEY_STR_VALUE;
    string a_name(device->getKeyInfo().value.strValue);
    a_name.append(":");
    a_name.append(attributeName);
    strcpy(kiv.value.strValue, a_name.c_str());
    
    device->getHasChildByKeyInfo(kiv, alreadyAdded);
    if(alreadyAdded) {
        vector<entity::Entity*> result;
        device->getChildsWithKeyInfo(kiv, result);
        return result[0];
    } else {
        return NULL;
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
    
    
    
    entity::Entity *device = getDeviceEntity(attributeDeviceID);
    
        //add the attribute
    edb::KeyIdAndValue kiv;
    kiv.keyID = keyIdDataset;
    kiv.type = chaos::edb::KEY_STR_VALUE;
    string a_name(attributeDeviceID);
    a_name.append(":");
    a_name.append(attributeName);
    strcpy(kiv.value.strValue, a_name.c_str());
    
    auto_ptr<entity::Entity> elementDst(entityDB->getNewEntityInstance(kiv));
    
    if(elementDst.get()) {
        elementDst->addStringProperty(keyIdAttrName, attributeName);
        elementDst->addStringProperty(keyIdAttrName, attributeDescription);
        elementDst->addNumberProperty(keyIdAttrType, attributeType);
        elementDst->addNumberProperty(keyIdAttrDir, attributeDirection);
        
        device->addChild(*elementDst);
    }
}

void CUSchemaDB::addUniqueAttributeStringProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, string& attributeValue) {
    ArrayPointer<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    bool found = keysAndValues.size() == 0;
    
    if(found) {
        found = false;
        for ( int idx = 0; idx< keysAndValues.size(); idx++) {
            if(!strcmp(keysAndValues[idx]->value.strValue, attributeValue.c_str())) {
                found = true;
                break;
            }
        }
    }
    
    if(!found) attributeEntity->addStringProperty(attributeEntity, attributeValue);
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, int64_t attributeValue) {
    ArrayPointer<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    
    bool found = keysAndValues.size() == 0;
    
    if(found) {
        found = false;
        for ( int idx = 0; idx< keysAndValues.size(); idx++) {
            if(keysAndValues[idx]->value.numValue == attributeValue) {
                found = true;
                break;
            }
        }
    }
    
    if(!found)  attributeEntity->addStringProperty(attributeEntity, attributeValue);
}

void CUSchemaDB::addUniqueAttributeProperty(entity::Entity *attributeEntity, uint32_t keyIDToAdd, double attributeValue) {
    ArrayPointer<edb::KeyIdAndValue> keysAndValues;
    attributeEntity->getPropertyByKeyID(keyIDToAdd, keysAndValues);
    
    bool found = keysAndValues.size() == 0;
    
    if(found) {
        found = false;
        for ( int idx = 0; idx< keysAndValues.size(); idx++) {
            if(keysAndValues[idx]->value.doubleValue == attributeValue) {
                found = true;
                break;
            }
        }
    }
    
    if(!found)  attributeEntity->addStringProperty(attributeEntity, attributeValue);
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
    
    if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID)){
        attributeDeviceID = attributeDataWrapper.getStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID);
    }
    
    if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION)){
            //get the entity for device
        auto_ptr<entity::Entity> deviceEntity(getDeviceEntity(attributeDeviceID));
        
        elementsDescriptions.reset(attributeDataWrapper.getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
        
        for (int idx = 0; idx < elementsDescriptions->size(); idx++) {
            
            
                //next element in dataset
            elementDescription.reset(elementsDescriptions->getCDataWrapperElementAtIndex(idx));
                //attribute name
            
            if(!elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME))
                continue;
            
            
            string attrName = elementDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME);
            
                //get the attribute
            auto_ptr<entity::Entity> attributeEntity(getDatasetElement(deviceEntity.get(), attrName));
            
            
                //attribute description
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION)){
                addUniqueAttributeProperty(keyIdAttrDesc, elementDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION));
            }
                //attribute type
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE)) {
                addUniqueAttributeProperty(keyIdAttrType, (int64_t)elementDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE));
            }
                //attribute direction
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION)){
                addUniqueAttributeProperty(keyIdAttrDir, (int64_t)elementDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION));
            }
            
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE)) {
                addUniqueAttributeProperty(keyIdAttrMaxRng, elementDescription->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE));
            }
            
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE)){
                addUniqueAttributeProperty(keyIdAttrMinRng, elementDescription->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE));
            }
            
                //add filled element to dataset
            deviceDataset.push_back(attributeForDeviceID);
        }
    }
}

/*
 fill a CDataWrapper with the dataset decode
 */
void CUSchemaDB::fillDataWrpperWithDataSetDescirption(CDataWrapper& datasetDescription) {
        //now i must describe the param for this action
        //CHAOS_ASSERT(datasetDescription)
    if(deviceIDDataset.size()){
        shared_ptr<CDataWrapper> domainDatasetDescription;
            //there are some parameter for this action, need to be added to rapresentation
        for (map<string, vector< CDataWrapper* > > ::iterator datasetIter = deviceIDDataset.begin();
             datasetIter != deviceIDDataset.end();
             datasetIter++) {
            
                //get domain name
            string domainName = datasetIter->first;
            
                //det dataset attribute for domain name
            
            vector<CDataWrapper*>& domainAttributeList = datasetIter->second;
            
            domainDatasetDescription.reset(new CDataWrapper());
                //add domain name to description data
            domainDatasetDescription->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, domainName);
                //add description for all attribute in the domain
            for (vector<CDataWrapper*>::iterator datasetIterator = domainAttributeList.begin();
                 datasetIterator != domainAttributeList.end();
                 datasetIterator++) {
                    // add parametere representation object to main action representation
                domainDatasetDescription->appendCDataWrapperToArray(**datasetIterator);
                    //CDataWrapper *data = *datasetIterator;
            }
            domainDatasetDescription->finalizeArrayForKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION);
                // add parametere representation object to main action representation
            datasetDescription.appendCDataWrapperToArray(*domainDatasetDescription.get());
        }
        
            //cloese the array
        datasetDescription.finalizeArrayForKey(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION);
    }
}

/*
 * Return the attribute array for a specified direction
 */
void CUSchemaDB::getAttributeForDirection(string& deviceID, DataType::DataSetAttributeIOAttribute attributeDiretion, vector< shared_ptr<CDataWrapper> >& resultVector) {
    
        //get the attribute list for domain
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(deviceID);
    for (vector<CDataWrapper*>::iterator datasetAttributeIterator = domainAttributeList.begin();
         datasetAttributeIterator != domainAttributeList.end();
         datasetAttributeIterator++) {
        
        if((*datasetAttributeIterator)->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION)) {
            int32_t direction = (*datasetAttributeIterator)->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION);
            if(direction==DataType::Bidirectional)
                resultVector.push_back(shared_ptr<CDataWrapper>((*datasetAttributeIterator)->clone()));
        }
    }
}

/*
 return al domain
 */
void CUSchemaDB::getAllDeviceId(vector<string>& domainNames) {
    for (map<string, vector<CDataWrapper*> > ::iterator deviceIDDatasetIter = deviceIDDataset.begin();
         deviceIDDatasetIter != deviceIDDataset.end();
         deviceIDDatasetIter++) {
        
            //add domain name
        domainNames.push_back(deviceIDDatasetIter->first);
    }
}

/*
 return al domain
 */
bool CUSchemaDB::deviceIsPresent(const string& deviceID) {
    for (map<string, vector<CDataWrapper*> > ::iterator deviceIDDatasetIter = deviceIDDataset.begin();
         deviceIDDatasetIter != deviceIDDataset.end();
         deviceIDDatasetIter++) {
        
            //add domain name
        if(!deviceIDDatasetIter->first.compare(deviceID)) return true;
    }
    return false;
}

void CUSchemaDB::getDeviceDatasetAttributesName(const string& deviceID, vector<string>& attributesName) {
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(deviceID);
    
    for (vector<CDataWrapper*>::iterator iter = domainAttributeList.begin();
         iter != domainAttributeList.end();
         iter++) {
        
        attributesName.push_back((*iter)->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME));
    }
}

void CUSchemaDB::getDeviceDatasetAttributesName(const string& deviceID,  DataType::DataSetAttributeIOAttribute directionType, vector<string>& attributesName) {
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(deviceID);
    DataType::DataSetAttributeIOAttribute type = DataType::Input;
    for (vector<CDataWrapper*>::iterator iter = domainAttributeList.begin();
         iter != domainAttributeList.end();
         iter++) {
        type = (DataType::DataSetAttributeIOAttribute)(*iter)->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION);
        if(type == directionType || type== DataType::Bidirectional) {
            attributesName.push_back((*iter)->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME));
        }
    }
}

void CUSchemaDB::getDeviceAttributeDescription(const string& deviceID, const string& attributesName, string& attributeDescription) {
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(deviceID);
    CDataWrapper *tmpPtr = NULL;
    for (vector<CDataWrapper*>::iterator iter = domainAttributeList.begin();
         iter != domainAttributeList.end();
         iter++) {
        tmpPtr = *iter;
        if(tmpPtr->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME).compare(attributesName)==0){
            attributeDescription = tmpPtr->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION);
            break;
        }
    }
}

void CUSchemaDB::getDeviceAttributeRangeValueInfo(const string& deviceID, const string& attributesName, RangeValueInfo& rangeInfo) {
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(deviceID);
    CDataWrapper *tmpPtr = NULL;
    for (vector<CDataWrapper*>::iterator iter = domainAttributeList.begin();
         iter != domainAttributeList.end();
         iter++) {
        tmpPtr = *iter;
        if(tmpPtr->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME).compare(attributesName)==0){
            if(tmpPtr->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE)){
                rangeInfo.minRange = tmpPtr->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MIN_RANGE);
            }
            
            if(tmpPtr->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE)){
                rangeInfo.maxRange = tmpPtr->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_MAX_RANGE);
            }
            
            if(tmpPtr->hasKey(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE)){
                rangeInfo.defaultValue = tmpPtr->getStringValue(DatasetDefinitionkey::CS_CMDM_ACTION_DESC_DEFAULT_VALUE);
            }
            if(tmpPtr->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE)){
                rangeInfo.valueType = (DataType::DataType)tmpPtr->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE);
            }
            
            break;
        }
    }
}

int CUSchemaDB::getDeviceAttributeDirection(const string& deviceID, const string& attributesName, DataType::DataSetAttributeIOAttribute& directionType) {
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(deviceID);
    CDataWrapper *tmpPtr = NULL;
    for (vector<CDataWrapper*>::iterator iter = domainAttributeList.begin();
         iter != domainAttributeList.end();
         iter++) {
        tmpPtr = *iter;
        if(tmpPtr->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME).compare(attributesName)==0){
            if(tmpPtr->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION)){
                directionType = (DataType::DataSetAttributeIOAttribute)tmpPtr->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION);
                return 0;
            }else return -1;
            
            break;
        }
    }
    return 0;
}

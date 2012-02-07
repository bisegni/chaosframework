//
//  CUSchemaDB.cpp
//  ChaosFramework
//
//  Created by Bisegni Claudio on 14/07/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include "../global.h"
#include "CUSchemaDB.h"

using namespace chaos;
using namespace std;
using namespace boost;

CUSchemaDB::CUSchemaDB() {
    
}

CUSchemaDB::~CUSchemaDB() {
        //remove all dataset
    clearAllDatasetsVectors();
}

void CUSchemaDB::clearAllDatasetsVectors() {
    for (map<string, vector< CDataWrapper* > > ::iterator datasetIter = deviceIDDataset.begin();
         datasetIter != deviceIDDataset.end();
         datasetIter++) {
        
            //get domain name
        string domainName = datasetIter->first;
        clearDatasetForDeviceID(domainName);
    }
    deviceIDDataset.clear();
}

void CUSchemaDB::clearDatasetForDeviceID(string& deviceID){
        //det dataset attribute for domain name
    
    vector< CDataWrapper* >& domainAttributeList = getDatasetForDeviceID(deviceID);
    
    for (vector<CDataWrapper*>::iterator datasetIterator = domainAttributeList.begin(); 
         datasetIterator != domainAttributeList.end(); 
         datasetIterator++) {
        CDataWrapper *attribute = *datasetIterator;
        delete(attribute);
    }
    
    domainAttributeList.clear();
}

/*
 return the vector containing the atrtibute list for a domain
 */
vector< CDataWrapper* >& CUSchemaDB::getDatasetForDeviceID(string& deviceID) {
    if(deviceIDDataset.count(deviceID) > 0){
        return deviceIDDataset[deviceID];
    }
    
        //a new vector need to be added
    addDeviceId(deviceID);
    return deviceIDDataset[deviceID];
}


/*
 return the vector containing the atrtibute list for a domain
 */
void CUSchemaDB::addDeviceId(string domainName) {
    if(deviceIDDataset.count(domainName) > 0){
        return;
    }
        //a new vector need to be added
    vector<CDataWrapper*> newVector;
    deviceIDDataset.insert(make_pair<string, vector<CDataWrapper*> >(domainName, newVector));
}

/*
 Add the new field to the dataset
 */
void CUSchemaDB::addAttributeToDataSet(const char*const attributeDeviceID, 
		const char*const attributeName,
		const char*const attributeDescription,
		DataType::DataType attributeType,
        DataType::DataSetAttributeIOAttribute attributeDirection) {
    
    string aDom = attributeDeviceID;
    
    vector< CDataWrapper* >& deviceDataset = getDatasetForDeviceID(aDom);
    CDataWrapper *attributeForDeviceID = new CDataWrapper();
    deviceDataset.push_back(attributeForDeviceID);
    attributeForDeviceID->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, attributeName);
    attributeForDeviceID->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION, attributeDescription);
    attributeForDeviceID->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE, attributeType);
    attributeForDeviceID->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION, attributeDirection);
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
        vector< CDataWrapper* >& deviceDataset = getDatasetForDeviceID(attributeDeviceID);
		elementsDescriptions.reset(attributeDataWrapper.getVectorValue(DatasetDefinitionkey::CS_CM_DATASET_DESCRIPTION));
    
        for (int idx = 0; idx<= elementsDescriptions->size(); idx++) {
                //create new space for element
             CDataWrapper *attributeForDeviceID = new CDataWrapper();
            
                //next element in dataset
            elementDescription.reset(elementsDescriptions->getCDataWrapperElementAtIndex(idx));
                //attribute name
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME))
                attributeForDeviceID->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME, elementDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME).c_str());
                //attribute description
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION))
                attributeForDeviceID->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION, elementDescription->getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION).c_str());
                //attribute type
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE))
                attributeForDeviceID->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE, elementDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE));
                //attribute direction
            if(elementDescription->hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION))
                attributeForDeviceID->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION, elementDescription->getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION));

            
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
void CUSchemaDB::getAttributeForDirection(string& domainName, DataType::DataSetAttributeIOAttribute attributeDiretion, vector< shared_ptr<CDataWrapper> >& resultVector) {
    
        //get the attribute list for domain
    vector<CDataWrapper*>& domainAttributeList = getDatasetForDeviceID(domainName);
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
bool CUSchemaDB::deviceIsPresent(string& deviceID) {
    for (map<string, vector<CDataWrapper*> > ::iterator deviceIDDatasetIter = deviceIDDataset.begin();
         deviceIDDatasetIter != deviceIDDataset.end();
         deviceIDDatasetIter++) {
        
            //add domain name
        if(!deviceIDDatasetIter->first.compare(deviceID)) return true;
    }
    return false;
}

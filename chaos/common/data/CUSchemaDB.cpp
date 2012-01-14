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

/*
 return the vector containing the atrtibute list for a domain
 */
ptr_vector<DataSetAttribute>& CUSchemaDB::getDSAttributeListForDeviceID(string& domainName) {
    if(datasetAttributeMap.count(domainName) > 0){
        return datasetAttributeMap[domainName];
    }
    
        //a new vector need to be added
    addDeviceId(domainName);
    return datasetAttributeMap[domainName];
}

/*
 return the vector containing the atrtibute list for a domain
 */
void CUSchemaDB::addDeviceId(string domainName) {
    if(datasetAttributeMap.count(domainName) > 0){
        return;
    }
        //a new vector need to be added
    ptr_vector<DataSetAttribute> newVector;
    datasetAttributeMap.insert(make_pair<string, ptr_vector<DataSetAttribute> >(domainName, newVector));
}


/*
 Fill the CDataWrapper with the valu eof the attribute of the dataset
 */
void CUSchemaDB::fillCDataWrapperWithAttributeValues(DataSetAttribute *srcAttributeDS,CDataWrapper *destCDataWrapper) {
    CHAOS_ASSERT(srcAttributeDS && destCDataWrapper)
        //add thename of the parameter
    if(srcAttributeDS->attributeName.size())
        destCDataWrapper->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME,
                                                    srcAttributeDS->attributeName);
    
    if(srcAttributeDS->attributeTag.size())
        destCDataWrapper->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TAG,
                                          srcAttributeDS->attributeTag);
    
        //add the information about the parameter
    if(srcAttributeDS->attributeDescription.size())
        destCDataWrapper->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION,
                                                    srcAttributeDS->attributeDescription);
    
        //add the parameter type
    destCDataWrapper->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION,
                                               srcAttributeDS->attributeDirection);
    
        //add the parameter type
    destCDataWrapper->addInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE,
                                               srcAttributeDS->attributeType);
}

/*
 Add the new field to the dataset
 */
void CUSchemaDB::addAttributeToDataSet(const char*const attributeDomain, 
		const char*const attributeName,
		const char*const attributeDescription,
		DataType::DataType attributeType,
		DataSetAttributeIOAttribute attributeDirection) {
    
    string aDom = attributeDomain;
    ptr_vector<DataSetAttribute>& attributeList = getDSAttributeListForDeviceID(aDom);
    
	DataSetAttribute *attribute = new DataSetAttribute();
	if(attributeName)attribute->attributeName.assign(attributeName);
	if(attributeDescription)attribute->attributeDescription.assign(attributeDescription);
	attribute->attributeType = attributeType;
	attribute->attributeDirection = attributeDirection;
    
    attributeList.push_back(attribute);
}

/*
   Add the new field at the CU dataset from the CDataWrapper
 */
void CUSchemaDB::addAttributeToDataSetFromDataWrapper(CDataWrapper& attributeDataWrapper) {
        //if(!attributeDataWrapper) return;

	string attributeDomain;
	string attributeName;
	string attributeDescription;
	DataSetAttributeIOAttribute attributeDirection = Input;
	DataType::DataType attributeType = DataType::TYPE_INT32;

	if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DOMAIN)){
		attributeDomain = attributeDataWrapper.getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DOMAIN);
	}
    
	if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME)){
		attributeName = attributeDataWrapper.getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_NAME);
	}
	if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION)){
		attributeDescription = attributeDataWrapper.getStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DESCRIPTION);
	}
	if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE)){
		attributeType = (DataType::DataType)attributeDataWrapper.getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_TYPE);
	}
	if(attributeDataWrapper.hasKey(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION)){
		attributeDirection = (DataSetAttributeIOAttribute)attributeDataWrapper.getInt32Value(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DIRECTION);
	}
	//add the attribute to the dataset
	addAttributeToDataSet(attributeDomain.c_str(), attributeName.c_str(), attributeDescription.c_str(), attributeType, attributeDirection);
}

/*
 fill a CDataWrapper with the dataset decode
 */
void CUSchemaDB::fillDataWrpperWithDataSetDescirption(CDataWrapper& datasetDescription) {    
	//now i must describe the param for this action
    //CHAOS_ASSERT(datasetDescription)
	if(datasetAttributeMap.size()){
        auto_ptr<CDataWrapper> domainDatasetDescription;
		auto_ptr<CDataWrapper> attributeDatasetDescription;
		//there are some parameter for this action, need to be added to rapresentation
		for (map<string, ptr_vector<DataSetAttribute> > ::iterator datasetMapIter = datasetAttributeMap.begin();
				datasetMapIter != datasetAttributeMap.end();
				datasetMapIter++) {
            
                //get domain name
            string domainName = datasetMapIter->first;
                
                //det dataset attribute for domain name
            
            ptr_vector<DataSetAttribute>& domainAttributeList = datasetMapIter->second;

            domainDatasetDescription.reset(new CDataWrapper());
                //add domain name to description data
            domainDatasetDescription->addStringValue(DatasetDefinitionkey::CS_CM_DATASET_ATTRIBUTE_DOMAIN, domainName);
                //add description for all attribute in the domain
            for (ptr_vector<DataSetAttribute>::iterator datasetIterator = domainAttributeList.begin(); 
                 datasetIterator != domainAttributeList.end(); 
                 datasetIterator++) {
                
                    //allocate a new element
                attributeDatasetDescription.reset(new CDataWrapper());
                
                    //add all attribute to the description
                fillCDataWrapperWithAttributeValues((DataSetAttribute*)&(*datasetIterator), attributeDatasetDescription.get());
                
                    // add parametere representation object to main action representation
                domainDatasetDescription->appendCDataWrapperToArray(*attributeDatasetDescription.get());
            }
            domainDatasetDescription->finalizeArrayForKey(DatasetDefinitionkey::CS_CM_DATASET_DESC);
                // add parametere representation object to main action representation
			datasetDescription.appendCDataWrapperToArray(*domainDatasetDescription.get());
		}

		//cloese the array
		datasetDescription.finalizeArrayForKey(DatasetDefinitionkey::CS_CM_DATASET_DESC);
	}
}

/*
 * Return the attribute array for a specified direction
 */
void CUSchemaDB::getAttributeForDirection(string& domainName, DataSetAttributeIOAttribute attributeDiretion, vector<DataSetAttribute*>& resultVector) {
    
        //get the attribute list for domain
    ptr_vector<DataSetAttribute>& domainAttributeList = getDSAttributeListForDeviceID(domainName);
	for (ptr_vector<DataSetAttribute>::iterator datasetAttributeIterator = domainAttributeList.begin(); 
         datasetAttributeIterator != domainAttributeList.end(); 
         datasetAttributeIterator++) {
        
		if(datasetAttributeIterator->attributeDirection!=attributeDiretion && datasetAttributeIterator->attributeDirection!=Bidirectional)
			continue;

		//add the found attribute
		resultVector.push_back((DataSetAttribute*)&(*datasetAttributeIterator));
	}
}

/*
 return al domain 
 */
void CUSchemaDB::getAllDeviceId(vector<string>& domainNames) {
    for (map<string, ptr_vector<DataSetAttribute> > ::iterator datasetMapIter = datasetAttributeMap.begin();
         datasetMapIter != datasetAttributeMap.end();
         datasetMapIter++) {
        
            //add domain name
        domainNames.push_back(datasetMapIter->first);
    }
}

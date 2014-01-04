/*
 *	DatasetDB.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/DatasetDB.h>

using namespace chaos::common::data;

DatasetDB::DatasetDB(bool onMemory):CUSchemaDB(onMemory) {
    
}

DatasetDB::~DatasetDB() {
    
}

void DatasetDB::setDeviceID(std::string _deviceID) {
    deviceID = _deviceID;
}

const char * DatasetDB::getDeviceID() {
    return deviceID.c_str();
}

//! Add device dataset definitio by serialized form
void DatasetDB::addAttributeToDataSetFromDataWrapper(CDataWrapper& serializedDW) {
	if(!serializedDW.hasKey(DatasetDefinitionkey::DEVICE_ID)) return;
	if(!deviceID.length())
		setDeviceID(serializedDW.getStringValue(DatasetDefinitionkey::DEVICE_ID));
	else {
		std::string tmp_dev_id = serializedDW.getStringValue(DatasetDefinitionkey::DEVICE_ID);
		if(deviceID.compare(tmp_dev_id) != 0) return;
	}
    CUSchemaDB::addAttributeToDataSetFromDataWrapper(serializedDW);
}

//! Fill a CDataWrapper with a single device information
void DatasetDB::fillDataWrapperWithDataSetDescription(CDataWrapper& dw) {
    /* NOTE this need to be changed becase this funciton retuan many device but now all is
     single device centric. So we need to use a new bson structure for descibe the device, so we need a new root key*/
    CUSchemaDB::fillDataWrapperWithDataSetDescription(deviceID, dw);
}

//! Add dataset attribute
void DatasetDB::addAttributeToDataSet(const char*const attributeName,
                                           const char*const attributeDescription,
                                           DataType::DataType attributeType,
                                           DataType::DataSetAttributeIOAttribute attributeDirection,
                                           uint32_t maxSize) {
    CUSchemaDB::addAttributeToDataSet(deviceID.c_str(), attributeName, attributeDescription, attributeType, attributeDirection, maxSize);
}



//!Get dataset attribute names
void DatasetDB::getDatasetAttributesName(vector<string>& attributesName) {
    CUSchemaDB::getDeviceDatasetAttributesName(deviceID, attributesName);
}

//!Get device attribute name that has a specified direction
void DatasetDB::getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
                                              vector<string>& attributesName) {
    CUSchemaDB::getDeviceDatasetAttributesName(deviceID, directionType, attributesName);
}

//!Get  attribute description
void DatasetDB::getAttributeDescription(const string& attributesName,
                                             string& attributeDescription) {
    CUSchemaDB::getDeviceAttributeDescription(deviceID, attributesName, attributeDescription);
}

//!Get the value information for a specified attribute name
int DatasetDB::getAttributeRangeValueInfo(const string& attributesName,
                                                RangeValueInfo& rangeInfo) {
    return CUSchemaDB::getDeviceAttributeRangeValueInfo(deviceID, attributesName, rangeInfo);
}

//!Set the range values for an attribute of the device
void DatasetDB::setAttributeRangeValueInfo(const string& attributesName,
                                                RangeValueInfo& rangeInfo) {
    CUSchemaDB::setDeviceAttributeRangeValueInfo(deviceID, attributesName, rangeInfo);
}

//!Get the direction of an attribute
int DatasetDB::getAttributeDirection(const string& attributesName,
                                          DataType::DataSetAttributeIOAttribute& directionType) {
    return CUSchemaDB::getDeviceAttributeDirection(deviceID, attributesName, directionType);
}

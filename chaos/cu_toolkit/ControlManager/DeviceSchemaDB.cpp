/*
 *	DeviceSchemaDB.cpp
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

#include <chaos/cu_toolkit/ControlManager/DeviceSchemaDB.h>

using namespace chaos::cu;

DeviceSchemaDB::DeviceSchemaDB(bool onMemory):CUSchemaDB(onMemory) {
    
}

DeviceSchemaDB::~DeviceSchemaDB() {
    
}

void DeviceSchemaDB::setDeviceID(std::string _deviceID) {
    deviceID = _deviceID;
}

const char * DeviceSchemaDB::getDeviceID() {
    return deviceID.c_str();
}

//! Add device dataset definitio by serialized form
/*!
 Fill the database with the devices and their dataset with the content
 of the serialized data
 
 \param serializedDataset serialze dataset
 */
void DeviceSchemaDB::addAttributeToDataSetFromDataWrapper(CDataWrapper& serializedDW) {
    CUSchemaDB::addAttributeToDataSetFromDataWrapper(serializedDW);
}

//! Fill a CDataWrapper with a single device information
/*!
 fill a CDataWrapper with the dataset decode
 */
void DeviceSchemaDB::fillDataWrapperWithDataSetDescription(CDataWrapper& dw) {
    CUSchemaDB::fillDataWrapperWithDataSetDescription(deviceID, dw);
}

//! Add dataset attribute
/*!
 Add the new attribute to the deviceID dataset specifing
 the default parameter
 
 \param attributeName the name of the new attribute
 \param attributeDescription the description of the attribute
 \param attributeType the type of the new attribute
 \param attributeDirection the direction of the new attribute
 */
void DeviceSchemaDB::addAttributeToDataSet(const char*const attributeName,
                                           const char*const attributeDescription,
                                           DataType::DataType attributeType,
                                           DataType::DataSetAttributeIOAttribute attributeDirection) {
    CUSchemaDB::addAttributeToDataSet(deviceID.c_str(), attributeName, attributeDescription, attributeType, attributeDirection);
}



//!Get dataset attribute names
/*!
 Return all dataset attribute name
 \param attributesName the array that will be filled with the name
 */
void DeviceSchemaDB::getDatasetAttributesName(vector<string>& attributesName) {
    CUSchemaDB::getDeviceDatasetAttributesName(deviceID, attributesName);
}

//!Get device attribute name that has a specified direction
/*!
 Return all dataset attribute name
 \param directionType the direction for attribute filtering
 \param attributesName the array that will be filled with the name
 */
void DeviceSchemaDB::getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
                                              vector<string>& attributesName) {
    CUSchemaDB::getDeviceDatasetAttributesName(deviceID, directionType, attributesName);
}

//!Get  attribute description
/*!
 Return the dataset description
 \param attributesName the name of the attribute
 \param attributeDescription the returned description
 */
void DeviceSchemaDB::getAttributeDescription(const string& attributesName,
                                             string& attributeDescription) {
    CUSchemaDB::getDeviceAttributeDescription(deviceID, attributesName, attributeDescription);
}

//!Get the value information for a specified attribute name
/*!
 Return the range value for the attribute
 \param attributesName the name of the attribute
 \param rangeInfo the range and default value of the attribute
 */
void DeviceSchemaDB::getAttributeRangeValueInfo(const string& attributesName,
                                                RangeValueInfo& rangeInfo) {
    CUSchemaDB::getDeviceAttributeRangeValueInfo(deviceID, attributesName, rangeInfo);    
}

//!Get the direction of an attribute
/*!
 Return the direcion of the attribute
 \param attributesName the name of the attribute
 \param directionType the direction of the attribute
 */
int DeviceSchemaDB::getAttributeDirection(const string& attributesName,
                                          DataType::DataSetAttributeIOAttribute& directionType) {
    return CUSchemaDB::getDeviceAttributeDirection(deviceID, attributesName, directionType);
}

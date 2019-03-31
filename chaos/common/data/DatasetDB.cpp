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
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/DatasetDB.h>

#ifdef _WIN32
using namespace chaos;
#endif
using namespace chaos::common::data;


DatasetDB::DatasetDB(bool onMemory):CUSchemaDB(onMemory) {
    
}

DatasetDB::~DatasetDB() {
    
}

void DatasetDB::setDeviceID(const std::string &_device_id) {
    device_id = _device_id;
    //initialize entity
    getDeviceEntity(device_id);
}

const string & DatasetDB::getDeviceID() {
    return device_id;
}

//! Add device dataset definitio by serialized form
void DatasetDB::addAttributeToDataSetFromDataWrapper(CDataWrapper& serializedDW) {
    if(!serializedDW.hasKey(NodeDefinitionKey::NODE_UNIQUE_ID)) return;
    if(!device_id.length())
        setDeviceID(serializedDW.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID));
    else {
        std::string tmp_dev_id = serializedDW.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
        if(device_id.compare(tmp_dev_id) != 0) return;
    }
    CUSchemaDB::addAttributeToDataSetFromDataWrapper(serializedDW);
}

//! Fill a CDataWrapper with a single device information
void DatasetDB::fillDataWrapperWithDataSetDescription(CDataWrapper& dw) {
    /* NOTE this need to be changed becase this funciton retuan many device but now all is
     single device centric. So we need to use a new bson structure for descibe the device, so we need a new root key*/
    CUSchemaDB::fillDataWrapperWithDataSetDescription(device_id, dw);
}

//! Add dataset attribute
void DatasetDB::addAttributeToDataSet(const std::string& attribute_name,
                                      const std::string& attribute_description,
                                      DataType::DataType attribute_type,
                                      DataType::DataSetAttributeIOAttribute attribute_direction,
                                      uint32_t maxs_sze) {
    CUSchemaDB::addAttributeToDataSet(device_id,
                                      attribute_name,
                                      attribute_description,
                                      attribute_type,
                                      attribute_direction,
                                      maxs_sze);
}

void DatasetDB::addBinaryAttributeAsSubtypeToDataSet(const std::string& attribute_name,
                                                     const std::string& attribute_description,
                                                     DataType::BinarySubtype               subtype,
                                                     int32_t    cardinality,
                                                     DataType::DataSetAttributeIOAttribute attribute_direction) {
    CUSchemaDB::addBinaryAttributeAsSubtypeToDataSet(device_id,
                                                     attribute_name,
                                                     attribute_description,
                                                     subtype,
                                                     cardinality,
                                                     attribute_direction);
}

void DatasetDB::addBinaryAttributeAsSubtypeToDataSet(const std::string&             attribute_name,
                                                     const std::string&             attribute_description,
                                                     const std::vector<int32_t>&    subtype_list,
                                                     int32_t                        cardinality,
                                                     DataType::DataSetAttributeIOAttribute attribute_direction) {
    CUSchemaDB::addBinaryAttributeAsSubtypeToDataSet(device_id,
                                                     attribute_name,
                                                     attribute_description,
                                                     subtype_list,
                                                     cardinality,
                                                     attribute_direction);
}

void DatasetDB::addBinaryAttributeAsMIMETypeToDataSet(const std::string& attribute_name,
                                                      const std::string& attribute_description,
                                                      std::string mime_type,
                                                      DataType::DataSetAttributeIOAttribute attribute_direction) {
    CUSchemaDB::addBinaryAttributeAsMIMETypeToDataSet(device_id,
                                                      attribute_name,
                                                      attribute_description,
                                                      mime_type,
                                                      attribute_direction);
}

//!Get dataset attribute names
void DatasetDB::getDatasetAttributesName(vector<string>& attributesName) {
    CUSchemaDB::getDeviceDatasetAttributesName(device_id, attributesName);
}

//!Get device attribute name that has a specified direction
void DatasetDB::getDatasetAttributesName(DataType::DataSetAttributeIOAttribute directionType,
                                         vector<string>& attributesName) {
    CUSchemaDB::getDeviceDatasetAttributesName(device_id, directionType, attributesName);
}

//!Get  attribute description
void DatasetDB::getAttributeDescription(const string& attributesName,
                                        string& attributeDescription) {
    CUSchemaDB::getDeviceAttributeDescription(device_id, attributesName, attributeDescription);
}

//!Get the value information for a specified attribute name
int DatasetDB::getAttributeRangeValueInfo(const string& attributesName,
                                          RangeValueInfo& rangeInfo) {
    return CUSchemaDB::getDeviceAttributeRangeValueInfo(device_id, attributesName, rangeInfo);
}

//!Set the range values for an attribute of the device
void DatasetDB::setAttributeRangeValueInfo(const string& attributesName,
                                           RangeValueInfo& rangeInfo) {
    CUSchemaDB::setDeviceAttributeRangeValueInfo(device_id, attributesName, rangeInfo);
}

//!Get the direction of an attribute
int DatasetDB::getAttributeDirection(const string& attributesName,
                                     DataType::DataSetAttributeIOAttribute& directionType) {
    return CUSchemaDB::getDeviceAttributeDirection(device_id, attributesName, directionType);
}

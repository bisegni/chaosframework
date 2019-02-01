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

#include <chaos/cu_toolkit/control_manager/script/api/EUDSValueManagement.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::cu::control_manager::script;
using namespace chaos::cu::control_manager::script::api;

#define EUSW_LAPP    INFO_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())
#define EUSW_DBG     DBG_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())
#define EUSW_LERR    ERR_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())

#define EUSW_ADD_DATASET_ATTRIBUTE      "addDatasetAttribute"
#define EUSW_GET_OUTPUT_ATTRIBUTE_VALUE "getOutputAttributeValue"
#define EUSW_SET_OUTPUT_ATTRIBUTE_VALUE "setOutputAttributeValue"
#define EUSW_GET_INPUT_ATTRIBUTE_VALUE  "getInputAttributeValue"

EUDSValueManagement::EUDSValueManagement(ScriptableExecutionUnit *_eu_instance):
TemplatedAbstractScriptableClass(this,
                                 "eu"),
eu_instance(_eu_instance){
    //add all exposed api
    addApi(EUSW_ADD_DATASET_ATTRIBUTE, &EUDSValueManagement::addDatasetAttribute);
    addApi(EUSW_SET_OUTPUT_ATTRIBUTE_VALUE, &EUDSValueManagement::setOutputAttributeValue);
    addApi(EUSW_GET_OUTPUT_ATTRIBUTE_VALUE, &EUDSValueManagement::getOutputAttributeValue);
    addApi(EUSW_GET_INPUT_ATTRIBUTE_VALUE, &EUDSValueManagement::getInputAttributeValue);
}

EUDSValueManagement::~EUDSValueManagement() {}

int EUDSValueManagement::addDatasetAttribute(const ScriptInParam& input_parameter,
                                             ScriptOutParam& output_parameter) {
    if(input_parameter.size() == 4 ||
       input_parameter.size() == 5) {
        return -1;
    }
    const std::string attribute_name = input_parameter[0].asString();
    const std::string attribute_description = input_parameter[1].asString();
    DataType::DataType attribute_type = static_cast<DataType::DataType>(input_parameter[2].asInt32());
    DataType::DataSetAttributeIOAttribute attribute_direction = static_cast<DataType::DataSetAttributeIOAttribute>(input_parameter[3].asInt32());
    uint32_t maxSize = (uint32_t)(input_parameter.size() == 5 ?input_parameter[4].asInt32():0);
    EUSW_LAPP<< CHAOS_FORMAT("Call of addDatasetAttribute with %1% %2% %3% %4%", %attribute_name%attribute_description%attribute_type%attribute_direction);
    
    eu_instance->addAttributeToDataSet(attribute_name,
                                       attribute_description,
                                       attribute_type,
                                       attribute_direction,
                                       maxSize);
    return 0;
}

//! return the value of an dataset attribute
int EUDSValueManagement::setOutputAttributeValue(const chaos::common::script::ScriptInParam& input_parameter,
                                                 chaos::common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 2) {
        EUSW_LERR << "Bad number of parameter for setOutputAttributeValue";
        return -1;
    }
    try{
        eu_instance->setOutputAttributeValue(input_parameter[0].asString(),
                                             input_parameter[1]);
    } catch(...) {
        return -3;
    }
    return 0;
}

//! return the value of an dataset attribute
int EUDSValueManagement::getOutputAttributeValue(const chaos::common::script::ScriptInParam& input_parameter,
                                                 chaos::common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 1) {
        EUSW_LERR << "Bad number of parameter for getOutputAttributeValue";
        return -1;
    }
    try{
        CDataVariant attribute_value = eu_instance->getOutputAttributeValue(input_parameter[0].asString());
        if(attribute_value.isValid() == false) return -2;
        output_parameter.push_back(attribute_value);
    } catch(...) {
        return -3;
    }
    return 0;
}

int EUDSValueManagement::getInputAttributeValue(const chaos::common::script::ScriptInParam& input_parameter,
                                                chaos::common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 1) {
        EUSW_LERR << "Bad number of parameter for getInputAttributeValue";
        return -1;
    }
    try{
        CDataVariant attribute_value = eu_instance->getInputAttributeValue(input_parameter[0].asString());
        if(attribute_value.isValid() == false) return -2;
        output_parameter.push_back(attribute_value);
    } catch(...) {
        return -3;
    }
    return 0;
}

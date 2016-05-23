/*
 *	EUScriptableWrapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 13/05/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/script/EUScriptableWrapper.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::script;
using namespace chaos::cu::control_manager::script;

#define EUSW_LAPP    INFO_LOG(EUScriptableWrapper) << eu_instance->getCUID() << " - "
#define EUSW_DBG     DBG_LOG(EUScriptableWrapper) << eu_instance->getCUID() << " - "
#define EUSW_LERR    ERR_LOG(EUScriptableWrapper) << eu_instance->getCUID() << " - "


EUScriptableWrapper::EUScriptableWrapper(ScriptableExecutionUnit *_eu_instance):
TemplatedAbstractScriptableClass(this,
                                 "eu"),
eu_instance(_eu_instance){
    //add all exposed api
    addApi(EUSW_ADD_DATASET_ATTRIBUTE, &EUScriptableWrapper::addDatasetAttribute);
}

EUScriptableWrapper::~EUScriptableWrapper() {}

int EUScriptableWrapper::addDatasetAttribute(const ScriptInParam& input_parameter,
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

int EUScriptableWrapper::addVariable(const ScriptInParam& input_parameter,
                                     ScriptOutParam& output_parameter){
    EUSW_LAPP<<"Call of addVariable";
    return 0;
}
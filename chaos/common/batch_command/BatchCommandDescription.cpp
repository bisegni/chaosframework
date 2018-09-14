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
#include <chaos/common/batch_command/BatchCommand.h>
#include <chaos/common/batch_command/BatchCommandDescription.h>
using namespace chaos::common::data;
using namespace chaos::common::batch_command;


BatchCommandDescription::BatchCommandDescription(const std::string& _command_alias,
                                                 const std::string& _command_description,
                                                 const std::string& _unique_identifier):
alias(_command_alias),
description(_command_description),
unique_identifier(_unique_identifier){}

BatchCommandDescription::~BatchCommandDescription(){}

void BatchCommandDescription::setInstancer(chaos::common::utility::ObjectInstancer<BatchCommand> *_instancer) {
    instancer.reset(_instancer);
}

void BatchCommandDescription::addParameter(const std::string& parameter_name,
                                           const std::string& parameter_description,
                                           chaos::DataType::DataType type,
                                           int32_t flag) {
    //allocate paramter
    ChaosSharedPtr< CDataWrapper > attribute_description(new CDataWrapper());
    
    attribute_description->addStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME, parameter_name);
    attribute_description->addStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION, parameter_description);
    attribute_description->addInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE, type);
    attribute_description->addInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG, flag);
    //insert attribute
    map_parameter.insert(make_pair(parameter_name, attribute_description));
}

ChaosSharedPtr<CDataWrapper>
BatchCommandDescription::getFullDescription() {
    ChaosSharedPtr<CDataWrapper> description_obj(new CDataWrapper());
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, unique_identifier);
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, alias);
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION, description);
    if(map_parameter.size()) {
        for(BCParamterMapIterator it = map_parameter.begin();
            it != map_parameter.end();
            it++) {
            description_obj->appendCDataWrapperToArray(*it->second);
        }
        description_obj->finalizeArrayForKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS);
    }
    return description_obj;
}

void BatchCommandDescription::getParameters(std::vector<std::string>& parameter_list) {
    for(BCParamterMapIterator it = map_parameter.begin();
        it != map_parameter.end();
        it++) {
        parameter_list.push_back(it->first);
    }
}

bool BatchCommandDescription::getParameterType(const std::string& parameter_name,
                                               chaos::DataType::DataType& type) {
    ChaosSharedPtr<CDataWrapper> desc(getParameterDescription(parameter_name));
    if(desc.get() == NULL) return false;
    type = (chaos::DataType::DataType)desc->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE);
    return true;
}


bool BatchCommandDescription::getParameterFlag(const std::string& parameter_name,
                                               int32_t& flag) {
    ChaosSharedPtr<CDataWrapper> desc(getParameterDescription(parameter_name));
    if(desc.get() == NULL) return false;
    flag = desc->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG);
    return true;
}


bool BatchCommandDescription::getParameterDescription(const std::string& parameter_name,
                                                      std::string& parameter_description) {
    ChaosSharedPtr<CDataWrapper> desc(getParameterDescription(parameter_name));
    if(desc.get() == NULL) return false;
    parameter_description = desc->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION);
    return true;
}

ChaosSharedPtr<CDataWrapper>
BatchCommandDescription::getParameterDescription(const std::string& parameter_name) {
    if(map_parameter.count(parameter_name)==0) return ChaosSharedPtr<CDataWrapper>();
    return map_parameter[parameter_name];
}

void BatchCommandDescription::setAlias(const std::string& _alias) {
    alias = _alias;
}

//! return the alias of the command
const std::string& BatchCommandDescription::getAlias() {
    return alias;
}

void BatchCommandDescription::setDescription(const std::string& _description) {
    description = _description;
}

//! return the alias of the command
const std::string& BatchCommandDescription::getDescription() {
    return description;
}

BatchCommand* BatchCommandDescription::getInstance() {
    BatchCommand *result = instancer->getInstance();
    if(result) {
        //copy the instancer attribute into new instance
        result->setInstanceCustomAttribute(instance_custom_attribute);
    }
    return result;
}

BCInstantiationAttributeMap& BatchCommandDescription::getCustomAttributeRef() {
    return instance_custom_attribute;
}

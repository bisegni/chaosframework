/*
 *	BatchCommandDescription.cpp
 *	!CHAOS
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

#include <chaos/common/batch_command/BatchCommandDescription.h>
using namespace chaos::common::data;
using namespace chaos::common::batch_command;


BatchCommandDescription::BatchCommandDescription(const std::string& _command_alias,
                                                 const std::string& _command_description,
                                                 const std::string& _unique_identifier):
alias(_command_alias),
description(_command_description),
unique_identifier(_unique_identifier),
instancer(NULL) {}

BatchCommandDescription::~BatchCommandDescription(){
    if(instancer) delete(instancer);
    instancer = NULL;
}

void BatchCommandDescription::setInstancer(chaos::common::utility::ObjectInstancer<BatchCommand> *_instancer) {
    if(instancer){
        delete (instancer);
    }
    instancer = _instancer;
}

void BatchCommandDescription::addParameter(const std::string& parameter_name,
                                           const std::string& parameter_description,
                                           chaos::DataType::DataType type,
                                           int32_t flag) {
    //allocate paramter
    boost::shared_ptr< CDataWrapper > attribute_description(new CDataWrapper());
    
    attribute_description->addStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME, parameter_name);
    attribute_description->addStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION, parameter_description);
    attribute_description->addInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE, type);
    attribute_description->addInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG, flag);
    //insert attribute
    map_parameter.insert(make_pair(parameter_name, attribute_description));
}

boost::shared_ptr<CDataWrapper>
BatchCommandDescription::getFullDescription() {
    boost::shared_ptr<CDataWrapper> description_obj(new CDataWrapper());
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_UNIQUE_ID, unique_identifier);
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, alias);
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION, description);
    if(map_parameter.size()) {
        for(BatchCommandMapParamterIterator it = map_parameter.begin();
            it != map_parameter.end();
            it++) {
            description_obj->appendCDataWrapperToArray(*it->second);
        }
        description_obj->finalizeArrayForKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS);
    }
    return description_obj;
}

void BatchCommandDescription::getParameters(std::vector<std::string>& parameter_list) {
    for(BatchCommandMapParamterIterator it = map_parameter.begin();
        it != map_parameter.end();
        it++) {
        parameter_list.push_back(it->first);
    }
}

bool BatchCommandDescription::getParameterType(const std::string& parameter_name,
                                               chaos::DataType::DataType& type) {
    boost::shared_ptr<CDataWrapper> desc(getParameterDescription(parameter_name));
    if(desc.get() == NULL) return false;
    type = (chaos::DataType::DataType)desc->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE);
    return true;
}


bool BatchCommandDescription::getParameterFlag(const std::string& parameter_name,
                                               int32_t& flag) {
    boost::shared_ptr<CDataWrapper> desc(getParameterDescription(parameter_name));
    if(desc.get() == NULL) return false;
    flag = desc->getInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_FLAG);
    return true;
}


bool BatchCommandDescription::getParameterDescription(const std::string& parameter_name,
                                                      std::string& parameter_description) {
    boost::shared_ptr<CDataWrapper> desc(getParameterDescription(parameter_name));
    if(desc.get() == NULL) return false;
    parameter_description = desc->getStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION);
    return true;
}

boost::shared_ptr<CDataWrapper>
BatchCommandDescription::getParameterDescription(const std::string& parameter_name) {
    if(map_parameter.count(parameter_name)==0) return boost::shared_ptr<CDataWrapper>();
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

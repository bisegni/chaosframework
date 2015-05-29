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

std::map<std::string, BatchCommandDescription* > BatchCommandDescription::global_description_instances;

BatchCommandDescription::BatchCommandDescription(const std::string& batch_command_alias,
                                                 const std::string& batch_command_description):
alias(batch_command_alias),
description(batch_command_description){
    global_description_instances.insert(make_pair(batch_command_alias, this));
}

BatchCommandDescription::~BatchCommandDescription(){}

void BatchCommandDescription::addParameter(const std::string& parameter_name,
                                           const std::string& parameter_description,
                                           chaos::DataType::DataType type) {
    //allocate paramter
    boost::shared_ptr< chaos::common::data::CDataWrapper > attribute_description(new CDataWrapper());
    
    attribute_description->addStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_NAME, parameter_name);
    attribute_description->addStringValue(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_DESCRIPTION, parameter_description);
    attribute_description->addInt32Value(BatchCommandAndParameterDescriptionkey::BC_PARAMETER_TYPE, type);
    
    //insert attribute
    map_parameter.insert(make_pair(parameter_name, attribute_description));
}

boost::shared_ptr<chaos::common::data::CDataWrapper>
BatchCommandDescription::getDescription() {
    boost::shared_ptr<chaos::common::data::CDataWrapper> description_obj(new CDataWrapper());
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_ALIAS, alias);
    description_obj->addStringValue(BatchCommandAndParameterDescriptionkey::BC_DESCRIPTION, description);
    if(map_parameter.size()) {
        for(MapParamterIterator it = map_parameter.begin();
            it != map_parameter.end();
            it++) {
            description_obj->appendCDataWrapperToArray(*it->second);
        }
        description_obj->finalizeArrayForKey(BatchCommandAndParameterDescriptionkey::BC_PARAMETERS);
    }
    return description_obj;
}

//! return the alias of the command
const std::string& BatchCommandDescription::getAlias() {
    return alias;
}
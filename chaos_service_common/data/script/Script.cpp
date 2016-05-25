/*
 *	Script.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#include "Script.h"

using namespace chaos::common::data;

using namespace chaos::service_common::data::script;

#pragma mark ScriptBaseDescription

ScriptBaseDescription::ScriptBaseDescription():
unique_id(0),
name(),
description(){}

ScriptBaseDescription::ScriptBaseDescription(const ScriptBaseDescription& copy_src):
unique_id(copy_src.unique_id),
name(copy_src.name),
description(copy_src.description){}

ScriptBaseDescription& ScriptBaseDescription::operator=(ScriptBaseDescription const &rhs) {
    unique_id = rhs.unique_id;
    name = rhs.name;
    description = rhs.description;
    return *this;
};

#pragma mark ScriptBaseDescriptionHelper

ScriptBaseDescriptionHelper::ScriptBaseDescriptionHelper():
TemplatedDataContainer<ScriptBaseDescription>(){}

ScriptBaseDescriptionHelper::ScriptBaseDescriptionHelper(const ScriptBaseDescription& copy_source):
TemplatedDataContainer<ScriptBaseDescription>(copy_source){}

ScriptBaseDescriptionHelper::ScriptBaseDescriptionHelper(CDataWrapper *serialized_data):
TemplatedDataContainer<ScriptBaseDescription>(serialized_data){
    deserialize(serialized_data);
}


void ScriptBaseDescriptionHelper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    container().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, SBD_NAME, "");
    container().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, SBD_DESCRIPTION, "");
}

std::auto_ptr<CDataWrapper> ScriptBaseDescriptionHelper::serialize(const uint64_t sequence) {
    std::auto_ptr<CDataWrapper> data_serialized(new CDataWrapper());
    data_serialized->addInt64Value("seq", (sequence?sequence:container().unique_id));
    data_serialized->addStringValue(SBD_NAME, container().name);
    data_serialized->addStringValue(SBD_DESCRIPTION, container().description);
    return data_serialized;
}

#pragma mark Script

Script::Script():
script_description(),
script_content(){}

Script::Script(const Script& copy_src):
script_description(copy_src.script_description),
script_content(copy_src.script_content){}

Script& Script::operator=(Script const &rhs) {
    script_description = rhs.script_description;
    script_content = rhs.script_content;
    return *this;
};

#pragma mark ScriptHelper

ScriptHelper::ScriptHelper():
TemplatedDataContainer<Script>(),
sdh(){}

ScriptHelper::ScriptHelper(const Script& copy_source):
TemplatedDataContainer<Script>(copy_source),
sdh(copy_source.script_description){}

ScriptHelper::ScriptHelper(CDataWrapper *serialized_data):
TemplatedDataContainer<Script>(serialized_data),
sdh(serialized_data){
    deserialize(serialized_data);
}

void ScriptHelper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    sdh.deserialize(serialized_data);
    //dcopy deserialize objet into own contained
    container().script_description = sdh.container();
    container().script_content = CDW_GET_SRT_WITH_DEFAULT(serialized_data, SBD_SCRIPT_CONTENT, "");
}

std::auto_ptr<CDataWrapper> ScriptHelper::serialize(const uint64_t sequence) {
    std::auto_ptr<CDataWrapper> data_serialized = sdh.serialize(sequence);
    data_serialized->addStringValue(SBD_SCRIPT_CONTENT, container().script_content);
    return data_serialized;
}
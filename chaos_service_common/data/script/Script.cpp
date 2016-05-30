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
description(),
language(){}

ScriptBaseDescription::ScriptBaseDescription(const ScriptBaseDescription& copy_src):
unique_id(copy_src.unique_id),
name(copy_src.name),
description(copy_src.description),
language(copy_src.language){}

ScriptBaseDescription& ScriptBaseDescription::operator=(ScriptBaseDescription const &rhs) {
    unique_id = rhs.unique_id;
    name = rhs.name;
    description = rhs.description;
    language = rhs.language;
    return *this;
};

#pragma mark ScriptBaseDescriptionSDWrapper

ScriptBaseDescriptionSDWrapper::ScriptBaseDescriptionSDWrapper():
TemplatedDataSDWrapper<ScriptBaseDescription>(){}

ScriptBaseDescriptionSDWrapper::ScriptBaseDescriptionSDWrapper(const ScriptBaseDescription& copy_source):
TemplatedDataSDWrapper<ScriptBaseDescription>(copy_source){}

ScriptBaseDescriptionSDWrapper::ScriptBaseDescriptionSDWrapper(CDataWrapper *serialized_data):
TemplatedDataSDWrapper<ScriptBaseDescription>(serialized_data){
    deserialize(serialized_data);
}


void ScriptBaseDescriptionSDWrapper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    dataWrapped().unique_id = (uint64_t)CDW_GET_INT64_WITH_DEFAULT(serialized_data, "seq", 0);
    dataWrapped().name = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_NAME, "");
    dataWrapped().description = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_DESCRIPTION, "");
    dataWrapped().language = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_LANGUAGE, "");
}

std::auto_ptr<CDataWrapper> ScriptBaseDescriptionSDWrapper::serialize(const uint64_t sequence) {
    std::auto_ptr<CDataWrapper> data_serialized(new CDataWrapper());
    data_serialized->addInt64Value("seq", (sequence?sequence:dataWrapped().unique_id));
    data_serialized->addStringValue(CHAOS_SBD_NAME, dataWrapped().name);
    data_serialized->addStringValue(CHAOS_SBD_DESCRIPTION, dataWrapped().description);
    data_serialized->addStringValue(CHAOS_SBD_LANGUAGE, dataWrapped().language);
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

#pragma mark ScriptSDWrapper

ScriptSDWrapper::ScriptSDWrapper():
TemplatedDataSDWrapper<Script>(),
sd_sdw(){}

ScriptSDWrapper::ScriptSDWrapper(const Script& copy_source):
TemplatedDataSDWrapper<Script>(copy_source),
sd_sdw(copy_source.script_description){}

ScriptSDWrapper::ScriptSDWrapper(CDataWrapper *serialized_data):
TemplatedDataSDWrapper<Script>(serialized_data),
sd_sdw(serialized_data){
    deserialize(serialized_data);
}

void ScriptSDWrapper::deserialize(CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    sd_sdw.deserialize(serialized_data);
    //dcopy deserialize objet into own contained
    dataWrapped().script_description = sd_sdw.dataWrapped();
    dataWrapped().script_content = CDW_GET_SRT_WITH_DEFAULT(serialized_data, CHAOS_SBD_SCRIPT_CONTENT, "");
}

std::auto_ptr<CDataWrapper> ScriptSDWrapper::serialize(const uint64_t sequence) {
    sd_sdw = dataWrapped().script_description;
    std::auto_ptr<CDataWrapper> data_serialized = sd_sdw.serialize(sequence);
    data_serialized->addStringValue(CHAOS_SBD_SCRIPT_CONTENT, dataWrapped().script_content);
    return data_serialized;
}
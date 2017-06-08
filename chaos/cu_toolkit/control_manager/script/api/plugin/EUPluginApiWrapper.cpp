/*
 *	EUPluginApiWrapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/06/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUPluginApiWrapper.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::data;
using namespace chaos::common::script;

using namespace chaos::cu::control_manager::script;
using namespace chaos::cu::control_manager::script::api;
using namespace chaos::cu::control_manager::script::api::plugin;

#define EUSW_LAPP    INFO_LOG_1_P(EUPluginApiWrapper, eu_instance->getCUID())
#define EUSW_DBG     DBG_LOG_1_P(EUPluginApiWrapper, eu_instance->getCUID())
#define EUSW_LERR    ERR_LOG_1_P(EUPluginApiWrapper, eu_instance->getCUID())

EUPluginApiWrapper::EUPluginApiWrapper(ScriptableExecutionUnit *_eu_instance,
                                       ChaosUniquePtr<EUAbstractApiPlugin>& _api_plugin):
TemplatedAbstractScriptableClass(this,
                                 _api_plugin->getApiName()),
eu_instance(_eu_instance),
api_plugin(_api_plugin){
    addApi("execute", &EUPluginApiWrapper::execPlugin);
}

EUPluginApiWrapper::~EUPluginApiWrapper() {}

int EUPluginApiWrapper::execPlugin(const ScriptInParam& input_parameter,
                           ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 2) {
        return -1;
    }
    
    int err = 0;
    try {
        const std::string input_json = input_parameter[0].asString();
        char *output_data = NULL;
        uint32_t output_size = 0;
        EUSW_DBG << CHAOS_FORMAT("Calling EU plugin with ---->%1%", %input_json);
        if((err = api_plugin->execute(input_json.c_str(), (uint32_t)input_json.size(), &output_data, &output_size)) == 0) {
            //we have data
            const std::string output_json((output_data?output_data:""), output_size);
            output_parameter.push_back(CDataVariant(CDataBuffer::newOwnBufferFromBuffer(output_data, output_size)));
            output_parameter.push_back(CDataVariant(output_size));
            EUSW_DBG << CHAOS_FORMAT("Result EU plugin with ---->%1%", %output_json);
        }
    } catch(...) {
        return -2;
    }
    return err;
}

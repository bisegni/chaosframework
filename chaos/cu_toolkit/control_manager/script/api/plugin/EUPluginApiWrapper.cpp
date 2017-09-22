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
                                       ChaosSharedPtr<EUAbstractApiPlugin>& _api_plugin):
TemplatedAbstractScriptableClass(this,
                                 _api_plugin->getApiName()),
eu_instance(_eu_instance),
api_plugin(_api_plugin){
    addApi("execute", &EUPluginApiWrapper::execPlugin);
}

EUPluginApiWrapper::~EUPluginApiWrapper() {}

int EUPluginApiWrapper::init(const char * init_data) {
    return api_plugin->init(init_data);
}

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

void EUPluginApiWrapper::deinit() {
    api_plugin->deinit();
}

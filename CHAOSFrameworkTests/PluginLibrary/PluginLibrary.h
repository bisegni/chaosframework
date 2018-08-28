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

#ifndef __CHAOSFrameworkTests_D30B815C_54CA_4E42_BE66_0720A6B92659_PluginLibrary_h
#define __CHAOSFrameworkTests_D30B815C_54CA_4E42_BE66_0720A6B92659_PluginLibrary_h

#define CHAOS_ENABLE_PLUGIN

#include <chaos/common/plugin/PluginDefinition.h>
#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUAbstractApiPlugin.h>

using namespace chaos::common::plugin;

class PluginLibrary:
public AbstractPlugin {
public:
    PluginLibrary();
    void test(int num);
};


//test the eu api in plugin
using namespace chaos::cu::control_manager::script::api::plugin;
DECLARE_EUAPI_PLUGIN_SCLASS(EUTestApiPLugin) {
    int init(const char * init_data);
    int execute(const char *in_data,
                uint32_t in_data_size,
                char **out_data,
                uint32_t *out_data_size);
    void deinit();
    const char *getApiName();
};


#endif /* __CHAOSFrameworkTests_D30B815C_54CA_4E42_BE66_0720A6B92659_PluginLibrary_h */

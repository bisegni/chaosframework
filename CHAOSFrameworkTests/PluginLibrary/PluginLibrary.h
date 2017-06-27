/*
 *	PluginLibrary.h
 *
 *	!CHAOS [CHAOSFrameworkTests]
 *	Created by bisegni.
 *
 *    	Copyright 23/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFrameworkTests_D30B815C_54CA_4E42_BE66_0720A6B92659_PluginLibrary_h
#define __CHAOSFrameworkTests_D30B815C_54CA_4E42_BE66_0720A6B92659_PluginLibrary_h

#define CHAOS_ENABLE_PLUGIN

#include <chaos/common/plugin/PluginDefinition.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>
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

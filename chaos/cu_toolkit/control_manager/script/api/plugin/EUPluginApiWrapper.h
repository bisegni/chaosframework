/*
 *	EUPluginApiWrapper.h
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

#ifndef __CHAOSFramework__939DF13_CFED_470E_B5D6_3BE700401715_EUPluginApiWrapper_h
#define __CHAOSFramework__939DF13_CFED_470E_B5D6_3BE700401715_EUPluginApiWrapper_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/script/AbstractScriptableClass.h>

#include <chaos/cu_toolkit/control_manager/script/api/plugin/EUAbstractApiPlugin.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {
                //! forward declaration
                class ScriptableExecutionUnit;
                namespace api {
                    namespace plugin {
                        //! class taht permit to interact with execution unit and chaos cript framework
                        class EUPluginApiWrapper:
                        public chaos::common::script::TemplatedAbstractScriptableClass<EUPluginApiWrapper> {
                            friend class chaos::cu::control_manager::script::ScriptableExecutionUnit;
                            
                            //execution unit instances
                            ScriptableExecutionUnit *eu_instance;
                            ChaosSharedPtr<plugin::EUAbstractApiPlugin> api_plugin;
                        public:
                            ~EUPluginApiWrapper();
                        protected:
                            EUPluginApiWrapper(ScriptableExecutionUnit *_eu_instance,
                                               ChaosSharedPtr<EUAbstractApiPlugin>& _api_plugin);
                            
                            int init(const char * init_data = NULL);
                            
                            int execPlugin(const chaos::common::script::ScriptInParam& input_parameter,
                                           chaos::common::script::ScriptOutParam& output_parameter);
                            
                            void deinit();
                        };
                    }
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__939DF13_CFED_470E_B5D6_3BE700401715_EUPluginApiWrapper_h */

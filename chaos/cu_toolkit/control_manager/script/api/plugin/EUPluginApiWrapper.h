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

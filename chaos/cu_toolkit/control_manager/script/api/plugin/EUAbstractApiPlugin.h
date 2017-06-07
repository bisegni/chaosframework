/*
 *	EUAbstractApiPlugin.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/06/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__FC8434B_3160_40F7_A040_90B7B48A3AC7_EUAbstractApiPlugin_h
#define __CHAOSFramework__FC8434B_3160_40F7_A040_90B7B48A3AC7_EUAbstractApiPlugin_h

#include <chaos/common/plugin/PluginDefinition.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace script {
                namespace api {
                    namespace plugin {
                        //!define the asbtract base class for implent EU api in plugin
                        class EUAbstractApiPlugin:
                        public chaos::common::plugin::AbstractPlugin {
                        public:
                            //!execute the api
                            /**
                             \param in_data json string null terminated
                             \param in_data_size optional size in byte of jsonin
                             \param out_data handle to a pointer allocated
                             \param out_data_size return size of jsonoutp to the  allocated, space
                             \return 0 if success, error otherwise
                             */
                            virtual int execute(const char *in_data,
                                                uint32_t in_data_size,
                                                char **out_data,
                                                uint32_t *out_data_size) = 0;
                            
                            virtual const char *getApiName() = 0;
                        };
                        
#define DECLARE_EUAPI_PLUGIN_SCLASS(CLASS_NAME)\
class CLASS_NAME:\
public chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin
                        
                        
#define DEFINE_EUAPI_PROTOTYPE(CLASS_NAME)\
DEFINE_PLUGIN_DEFINITION_PROTOTYPE(CLASS_NAME, chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin)
                        
                        //! Define a plugin for a driver
                        /*!
                         Define the class that implement the EUAbstractApiPlugin
                         \param a alias used to instantiate the driver
                         \param v the version of the driver
                         \param n the class that implement the AbstractDriver
                         */
#define OPEN_EUAPI_LUGIN_CLASS_DEFINITION(a, v, n)\
OPEN_GENERAL_PLUGIN_CLASS_DEFINITION(a, v, EUAPIDRV, n, chaos::cu::control_manager::script::api::plugin::EUAbstractApiPlugin)
                        
                        
#define REGISTER_EUAPI_CLASS_INIT_ATTRIBUTE REGISTER_PLUGIN_CLASS_INIT_ATTRIBUTE
                        
                        
#define CLOSE_EUAPI_CLASS_DEFINITION CLOSE_PLUGIN_CLASS_DEFINITION
                    }
                }
            }
        }
    }
}


#endif /* __CHAOSFramework__FC8434B_3160_40F7_A040_90B7B48A3AC7_EUAbstractApiPlugin_h */

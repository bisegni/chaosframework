/*
 *	plugin_constants.h
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

#ifndef __CHAOSFramework__20927F5_831A_4A1F_ACB0_628709223BD9_plugin_constants_h
#define __CHAOSFramework__20927F5_831A_4A1F_ACB0_628709223BD9_plugin_constants_h
namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace script {
                namespace api {
                    namespace plugin {
                        
#define EU_PLUGIN_DIRECTORY             "eu-plugin-directory"
#define EU_PLUGIN_DIRECTORY_DESC        "Specify the directory where execution unit api plugin are ocated"
#define EU_PLUGIN_DIRECTORY_DEFAULT     "plugin"

#define EU_PLUGIN_ENABLE                "eu-plugin-enable"
#define EU_PLUGIN_ENABLE_DESC           "Enable the usage of plugin api for execution unit"
#define EU_PLUGIN_ENABLE_DEFAULT        false
                    }
                }
            }
        }
    }
}
}#endif /* __CHAOSFramework__20927F5_831A_4A1F_ACB0_628709223BD9_plugin_constants_h */

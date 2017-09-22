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
                        
#define UNIT_GATEWAY_ENABLE             "unit-gateway-enable"
#define UNIT_GATEWAY_ENABLE_DESC        "Enable the management of external unit management"
#define UNIT_GATEWAY_ENABLE_DEFAULT      false
                    }
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__20927F5_831A_4A1F_ACB0_628709223BD9_plugin_constants_h */

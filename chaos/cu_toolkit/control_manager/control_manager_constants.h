/*
 *	control_manager_constants.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 30/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__662D840_D1D7_4828_94E6_0B287F8552F4_control_manager_constants_h
#define __CHAOSFramework__662D840_D1D7_4828_94E6_0B287F8552F4_control_manager_constants_h

#include <chaos/cu_toolkit/control_manager/execution_pool/execution_pool_constants.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            //startup parameter for unit server
#define CONTROL_MANAGER_UNIT_SERVER_ENABLE							"unit-server-enable"
#define CONTROL_MANAGER_UNIT_SERVER_ENABLE_desc						"Enable the unit server on this instance of process"
            
#define CONTROL_MANAGER_UNIT_SERVER_ALIAS							"unit-server-alias"
#define CONTROL_MANAGER_UNIT_SERVER_ALIAS_desc						"Alias used to publish the unit server"
#define CONTROL_MANAGER_UNIT_SERVER_KEY								"unit-server-file-key"
#define CONTROL_MANAGER_UNIT_SERVER_KEY_desc						"the path to the file that contains the rsa public key for the unit server alias"
#define CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC			"unit-server-retry-ms"
#define CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_desc	"Delay in milliseconds for the registration retry"
#define CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_DEFAULT	2000
        }
    }
}

#endif /* __CHAOSFramework__662D840_D1D7_4828_94E6_0B287F8552F4_control_manager_constants_h */

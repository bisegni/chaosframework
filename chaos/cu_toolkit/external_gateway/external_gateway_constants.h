/*
 *	external_gateway_constants.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 13/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_F67CDB4D_86BB_49E0_9859_CE6BDCC3C989_external_gateway_constants_h
#define __CHAOSFramework_F67CDB4D_86BB_49E0_9859_CE6BDCC3C989_external_gateway_constants_h

namespace chaos{
    namespace cu {
        namespace external_gateway {
            
#define CU_EG_OPT_WORKER_THREAD_NUMBER          "cu-eg-worker-thread-number"
#define CU_EG_OPT_WORKER_THREAD_NUMBER_DESC     "Specify the number ofr the thread that the external adapter work need to use"
#define CU_EG_OPT_WORKER_THREAD_NUMBER_DEFAULT  4

#define CU_EG_OPT_WORKER_KV_PARAM               "cu-eg-adapter-kv-param"
#define CU_EG_OPT_WORKER_KV_PARAM_DESC          "Abstraction key:value parameter used by external protocol adapter implementation"

        }
    }
}

#endif /* __CHAOSFramework_F67CDB4D_86BB_49E0_9859_CE6BDCC3C989_external_gateway_constants_h */

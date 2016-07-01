/*
 *	execution_pool_constants.h
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

#ifndef __CHAOSFramework__71F52C7_F977_4086_8294_15E4C905A1BB_execution_pool_constants_h
#define __CHAOSFramework__71F52C7_F977_4086_8294_15E4C905A1BB_execution_pool_constants_h
namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace execution_pool {
                
#define CONTROL_MANAGER_EXECUTION_POOLS                 "execution_pools"
#define CONTROL_MANAGER_EXECUTION_POOLS_DESC            "Is the lis tof execution pool to use for request job"
                
#define CONTROL_MANAGER_EXECUTION_POOLS_CHECK_TIME      15000
                
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP         "execution_pools_cpu_cap"
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT 70
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DESC    "Is the maximum percentage of cpu occupancy, used to check when unit server don't must ask anymore for new job"
                
            }
        }
    }
}
#endif /* __CHAOSFramework__71F52C7_F977_4086_8294_15E4C905A1BB_execution_pool_constants_h */

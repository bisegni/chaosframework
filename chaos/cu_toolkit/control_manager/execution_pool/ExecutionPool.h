/*
 *	ExecutionPool.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 20/06/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__1B9BEDD_3879_4B52_BCEA_66BB810F0975_ExecutionPool_h
#define __CHAOSFramework__1B9BEDD_3879_4B52_BCEA_66BB810F0975_ExecutionPool_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>

#define CONTROL_MANAGER_EXECUTION_POOLS                 "execution_pools"
#define CONTROL_MANAGER_EXECUTION_POOLS_DESC            "Is the lis tof execution pool to use for request job"

#define CONTROL_MANAGER_EXECUTION_POOLS_CHECK_TIME      30000

#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP         "execution_pools_cpu_cap"
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT 70
#define CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DESC    "Is the maximum percentage of cpu occupancy, used to check when unit server don't must ask anymore for new job"

namespace chaos{
    namespace cu {
        namespace control_manager {
            //!execution pool name space
            namespace execution_pool {
                
                //!This is the root class for the execution pools management
                /*!
                 IN !CHAOS an execution pool is a path into the group structure. A script
                 can belong ot one or many of this path. IN a simple way, unit server can
                 declas from wich execution pool it wan't load the script. This class
                 check the his host for resource available and if it has enought CPU 
                 free time it can request another script to run. MDS in this case check if
                 we have script instance that aren't assigne to any unit server, in this
                 case it send load event to this unit server for instantiate the script.
                 Execution pool manager constantly update the execution pool timestamp
                 for his script talking with mds. In this case all script with an expired 
                 hearbeat can be reassigned to other unit server for the same execution pool.
                 */
                class ExecutionPoolManager:
                public chaos::common::utility::InizializableService,
                public chaos::common::async_central::TimerHandler {
                    
                    //! contain all uid for the loaded execution unit
                    ChaosStringSet      eu_uid_list;
                    ChaosStringVector   execution_pool_list;
                    boost::mutex        mutex_uid_set;
                    double              cpu_cap_percentage;
                protected:
                    ExecutionPoolManager();
                    ~ExecutionPoolManager();
                    //!time handler inherited
                    void timeout();
                public:
                    void init(void *init_data) throw(chaos::CException);
                    void deinit() throw(chaos::CException);
                    
                    void registerUID(const std::string& new_uid);
                    void deregisterUID(const std::string& remove_uid);
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__1B9BEDD_3879_4B52_BCEA_66BB810F0975_ExecutionPool_h */

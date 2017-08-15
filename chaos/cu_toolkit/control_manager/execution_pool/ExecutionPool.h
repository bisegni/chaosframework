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

#ifndef __CHAOSFramework__1B9BEDD_3879_4B52_BCEA_66BB810F0975_ExecutionPool_h
#define __CHAOSFramework__1B9BEDD_3879_4B52_BCEA_66BB810F0975_ExecutionPool_h

#include <chaos/cu_toolkit/control_manager/execution_pool/execution_pool_constants.h>
#include <chaos/cu_toolkit/control_manager/control_manager_constants.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos/common/utility/InizializableService.h>

#include <boost/thread.hpp>


namespace chaos{
    namespace common {
        namespace message {
            class MDSMessageChannel;
        }
    }
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
                    std::string         unit_server_alias;
                    
                    chaos::common::message::MDSMessageChannel *mds_message_channel;
                protected:
                    //!time handler inherited
                    void timeout();
                public:
                    ExecutionPoolManager();
                    ~ExecutionPoolManager();
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

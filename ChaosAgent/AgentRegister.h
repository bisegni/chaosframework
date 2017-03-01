/*
 *	AgentRegister.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 03/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__1F03397_993E_4BE7_B812_4C9A5B83310A_AgentRegisterSM_h
#define __CHAOSFramework__1F03397_993E_4BE7_B812_4C9A5B83310A_AgentRegisterSM_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/LockableObject.h>

#include <map>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace agent {
        //!forward declaration
        class AbstractWorker;
        
        typedef boost::shared_ptr<AbstractWorker> WorkerSharedPtr;
        CHAOS_DEFINE_MAP_FOR_TYPE(std::string, WorkerSharedPtr, MapWorker);
        
        typedef enum {
            AgentRegisterStateUnregistered,
            AgentRegisterStateStartRegistering,
            AgentRegisterStateRegistering,
            AgentRegisterStateRegistered,
            AgentRegisterStateStartUnregistering,
            AgentRegisterStateUnregistering,
            AgentRegisterStateFault
        } AgentRegisterState;
        
        CHAOS_DEFINE_LOCKABLE_OBJECT(AgentRegisterState, AgentRegisterStateLockable);
        
        class AgentRegister:
        public chaos::DeclareAction,
        public chaos::common::utility::StartableService,
        public chaos::common::async_central::TimerHandler {
            const std::string agent_uid;
            const std::string rpc_domain;
            //agent container map
            MapWorker    map_worker;
            uint32_t    reg_retry_counter;
            uint32_t    max_reg_retry_counter;
            AgentRegisterStateLockable           registration_state;
            common::message::MDSMessageChannel   *mds_message_channel;
           
        protected:
            //!inherited by chaos::common::async_central::TimerHandler
            void timeout();
            std::auto_ptr<chaos::common::data::CDataWrapper> getAgentRegistrationPack();
            
            /*!
             receive the ack package for agent registration on the MDS
             */
            chaos::common::data::CDataWrapper* registrationACK(chaos::common::data::CDataWrapper  *message_data, bool& detach);
        public:
            AgentRegister();
            ~AgentRegister();
            void init(void *init_data) throw (chaos::CException);
            void start() throw (chaos::CException);
            void stop() throw (chaos::CException);
            void deinit() throw (chaos::CException);
            
            //!add a new agent
            void addWorker(WorkerSharedPtr new_worker);
        };
        
    }
}

#endif /* __CHAOSFramework__1F03397_993E_4BE7_B812_4C9A5B83310A_AgentRegisterSM_h */

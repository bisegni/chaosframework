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

#ifndef __CHAOSFramework__1F03397_993E_4BE7_B812_4C9A5B83310A_AgentRegisterSM_h
#define __CHAOSFramework__1F03397_993E_4BE7_B812_4C9A5B83310A_AgentRegisterSM_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/LockableObject.h>

#include <chaos_service_common/data/node/Agent.h>

#include <map>
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace agent {
        //!forward declaration
        class AbstractWorker;
        
        typedef ChaosSharedPtr<AbstractWorker> WorkerSharedPtr;
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
            const std::string rpc_domain;
            //agent container map
            MapWorker    map_worker;
            uint32_t    reg_retry_counter;
            uint32_t    max_reg_retry_counter;
            AgentRegisterStateLockable                                      registration_state;
            common::message::MDSMessageChannel                              *mds_message_channel;
            chaos::service_common::data::agent::AgentInstanceSDWrapper      agent_instance_sd_wrapper;
        protected:
            //!inherited by chaos::common::async_central::TimerHandler
            void timeout();
            ChaosUniquePtr<chaos::common::data::CDataWrapper> getAgentRegistrationPack();
            
            /*!
             receive the ack package for agent registration on the MDS
             */
            chaos::common::data::CDWUniquePtr registrationACK(chaos::common::data::CDWUniquePtr  message_data);
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

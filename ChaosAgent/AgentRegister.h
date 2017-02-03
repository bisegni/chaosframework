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

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/async_central/async_central.h>

namespace chaos {
    namespace agent {
        
        typedef enum {
            
        } AgentRegisterState;
        
        class AgentRegister:
        public chaos::DeclareAction,
        public chaos::common::async_central::TimerHandler {
        protected:
            AgentRegister();
            ~AgentRegister();
            //!inherited by chaos::common::async_central::TimerHandler
            void timeout();
        public:
            
            /*!
             receive the ack package for agent registration on the MDS
             */
            chaos::common::data::CDataWrapper* registrationACK(chaos::common::data::CDataWrapper  *message_data, bool& detach);
        };
        
    }
}

#endif /* __CHAOSFramework__1F03397_993E_4BE7_B812_4C9A5B83310A_AgentRegisterSM_h */

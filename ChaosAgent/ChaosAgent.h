/*
 *	ChaosAgent.h
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

#ifndef __CHAOSFramework__8AAB364_4023_4894_BA67_238857A1019B_ChaosAgent_h
#define __CHAOSFramework__8AAB364_4023_4894_BA67_238857A1019B_ChaosAgent_h

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include "chaos_agent_types.h"
#include "AgentRegister.h"

namespace chaos {
    namespace agent {
        
        //! main class for chaos agent process
        class ChaosAgent:
        public ChaosCommon<ChaosAgent>,
        public ServerDelegator {
            friend class common::utility::Singleton<ChaosAgent>;
            
            static WaitSemaphore wait_close_semaphore;
            common::utility::StartableServiceContainer<AgentRegister> agent_register;
        private:
            ChaosAgent();
            ~ChaosAgent();
            static void signalHanlder(int signal_number);
        public:
            ChaosAgentSettings settings;
            void init(int argc, char* argv[]) throw (CException);
            void init(istringstream &initStringStream) throw (CException);
            void init(void *init_data)  throw(CException);
            void start()throw(CException);
            void stop()throw(CException);
            void deinit()throw(CException);
        };
        
    }
}

#endif /* __CHAOSFramework__8AAB364_4023_4894_BA67_238857A1019B_ChaosAgent_h */

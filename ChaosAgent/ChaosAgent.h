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

#ifndef __CHAOSFramework__8AAB364_4023_4894_BA67_238857A1019B_ChaosAgent_h
#define __CHAOSFramework__8AAB364_4023_4894_BA67_238857A1019B_ChaosAgent_h

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include "chaos_agent_types.h"
#include "AgentRegister.h"
#include "external_command_pipe/ExternaCommandExecutor.h"



namespace chaos {
    namespace agent {
        namespace utility{
        class ProcRestUtil;
        };
        //! main class for chaos agent process
        class ChaosAgent:
        public ChaosCommon<ChaosAgent>,
        public ServerDelegator {
            friend class common::utility::Singleton<ChaosAgent>;
            
            static WaitSemaphore wait_close_semaphore;
            common::utility::StartableServiceContainer<AgentRegister> agent_register;
#ifdef OLD_PROCESS_MANAGEMENT
            common::utility::InizializableServiceContainer<external_command_pipe::ExternaCommandExecutor> external_cmd_executor;
#else
            static ChaosSharedPtr <utility::ProcRestUtil> procRestUtil;
            public:
            ChaosSharedPtr <utility::ProcRestUtil> getProcessManager();

#endif
        private:
            ChaosAgent();
            ~ChaosAgent();
            static void signalHanlder(int signal_number);
        public:
            ChaosAgentSettings settings;
            void init(int argc, const char* argv[]);
            void init(istringstream &initStringStream);
            void init(void *init_data);
            void start();
            void stop();
            void deinit();
        };
        
    }
}

#endif /* __CHAOSFramework__8AAB364_4023_4894_BA67_238857A1019B_ChaosAgent_h */

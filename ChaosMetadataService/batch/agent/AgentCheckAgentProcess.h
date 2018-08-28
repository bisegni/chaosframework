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

#ifndef __CHAOSFramework__F039C0C_8B0A_4E64_9672_F9A4588345A6_AgentCheckAgentProcess_h
#define __CHAOSFramework__F039C0C_8B0A_4E64_9672_F9A4588345A6_AgentCheckAgentProcess_h

#include "../mds_service_batch.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/event/event.h>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            
            namespace agent {
                
                class AgentCheckAgentProcess:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    std::string agent_uid;
                    ChaosUniquePtr<RequestInfo> request;
                    chaos::common::data::CDWUniquePtr message_data;
                    
                    chaos::common::event::channel::AlertEventChannel *alert_event_channel;
                public:
                    AgentCheckAgentProcess();
                    ~AgentCheckAgentProcess();
                protected:
                    // inherited method
                    void setHandler(chaos_data::CDataWrapper *data);
                    
                    // inherited method
                    void acquireHandler();
                    
                    // inherited method
                    void ccHandler();
                    
                    // inherited method
                    bool timeoutHandler();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__F039C0C_8B0A_4E64_9672_F9A4588345A6_AgentCheckAgentProcess_h */

/*
 *	AgentLoggingNodeManagement.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 07/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_DC7AF29A_419C_4E63_B3AB_6B7C3B2F52A4_AgentLoggingNodeManagement_h
#define __CHAOSFramework_DC7AF29A_419C_4E63_B3AB_6B7C3B2F52A4_AgentLoggingNodeManagement_h

#include "../mds_service_batch.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/event/event.h>
namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace agent {
                class AgentLoggingNodeManagement:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    std::string managed_node;
                    ChaosUniquePtr<RequestInfo> request;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> message_data;

                public:
                    AgentLoggingNodeManagement();
                    ~AgentLoggingNodeManagement();
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

#endif /* __CHAOSFramework_DC7AF29A_419C_4E63_B3AB_6B7C3B2F52A4_AgentLoggingNodeManagement_h */

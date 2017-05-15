/*
 *	AgentAckCommand.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 06/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__32B30CE_CBE0_42B5_BFCC_DB6846A10D7B_AgentAckCommand_h
#define __CHAOSFramework__32B30CE_CBE0_42B5_BFCC_DB6846A10D7B_AgentAckCommand_h

#include "../mds_service_batch.h"
#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            
            namespace agent {
                
                class AgentAckCommand:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    std::string node_uid;
                    std::auto_ptr<RequestInfo> request;
                    chaos::common::data::CDataWrapper *message_data;
                public:
                    AgentAckCommand();
                    ~AgentAckCommand();
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

#endif /* __CHAOSFramework__32B30CE_CBE0_42B5_BFCC_DB6846A10D7B_AgentAckCommand_h */

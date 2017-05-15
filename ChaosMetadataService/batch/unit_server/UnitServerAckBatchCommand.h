/*
 *	UnitServerAckCommand.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__UnitServerAckCommand__
#define __CHAOSFramework__UnitServerAckCommand__

#include "../mds_service_batch.h"
#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            
            namespace unit_server {
                
                typedef enum UnitServerAckPhase {
                    USAP_ACK_US,
                    USAP_CU_AUTOLOAD,
                    USAP_CU_FECTH_NEXT,
                    USAP_END
                } UnitServerAckPhase;
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(chaos::metadata_service::persistence::NodeSearchIndex, AutoloadCUList)
                
                class UnitServerAckCommand:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    persistence::NodeSearchIndex last_worked_cu;
                    std::string unit_server_uid;
                    UnitServerAckPhase phase;
                    
                    std::auto_ptr<RequestInfo> request;
                    std::auto_ptr<CDataWrapper> autoload_pack;
                    chaos::common::data::CDataWrapper *message_data;
                    
                    AutoloadCUList          list_autoload_cu;
                    AutoloadCUListIterator  list_autoload_cu_current;
                public:
                    UnitServerAckCommand();
                    ~UnitServerAckCommand();
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

#endif /* defined(__CHAOSFramework__UnitServerAckCommand__) */

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
                    
                    bool us_can_start;
                    std::string destination_address;
                    ChaosUniquePtr<RequestInfo> request;
                    chaos::common::data::CDWUniquePtr autoload_pack;
                    chaos::common::data::CDWUniquePtr message_data;
                    
                    AutoloadCUList          list_autoload_cu;
                    AutoloadCUListIterator  list_autoload_cu_current;
                    
                    int prepareInstance();
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

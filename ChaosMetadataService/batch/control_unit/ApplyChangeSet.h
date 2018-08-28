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

#ifndef __CHAOSFramework__ApplyChangeSet__
#define __CHAOSFramework__ApplyChangeSet__

#include "../mds_service_batch.h"

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace control_unit {
                
                //!Batch command for apply a change set to a control unit
                class ApplyChangeSet:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    uint32_t retry_number;
                    
                    CNetworkAddress * control_unit_address;
                    chaos::common::data::CDWUniquePtr message;
                    ChaosUniquePtr<RequestInfo> request;
                    
                    std::string cu_id;
                public:
                    ApplyChangeSet();
                    ~ApplyChangeSet();
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

#endif /* defined(__CHAOSFramework__ApplyChangeSet__) */

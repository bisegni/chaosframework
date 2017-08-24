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

#ifndef __CHAOSFramework__RestoreSnapshotBatch_h
#define __CHAOSFramework__RestoreSnapshotBatch_h

#include "../mds_service_batch.h"

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace general {
                
                typedef enum RestorePhase {
                    NEW_RESTORE_REQUEST,
                    MANAGE_RESTORE_REQUEST,
                    NO_MORE_NODE
                } RestorePhase;
                
                //!batch command for submit batch command within a node
                class RestoreSnapshotBatch:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    //request for the command submission
                    ChaosUniquePtr<RequestInfo> restore_request;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> restore_message;
                    std::vector<std::string> list_node_in_snapshot;
                    
                    std::string snapshot_name;
                    unsigned int node_index;
                    RestorePhase restore_phase;
                public:
                    RestoreSnapshotBatch();
                    ~RestoreSnapshotBatch();
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

#endif /* RestoreSnapshotBatch_hpp */

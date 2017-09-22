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

#ifndef __CHAOSFramework__88C5D13_DAD7_419F_ACBA_190123A754B0_CreateSnapshotBatch_h
#define __CHAOSFramework__88C5D13_DAD7_419F_ACBA_190123A754B0_CreateSnapshotBatch_h

#include "../mds_service_batch.h"

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace general {
                
                typedef enum SnapshotCreationPhase {
                    SnapshotCreationPhaseFetchDataset,
                    SnapshotCreationPhaseStoreDataset,
                    SnapshotCreationPhaseEnd
                } SnapshotCreationPhase;
                
                //!batch command for submit batch command within a node
                class CreateSnapshotBatch:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    std::string work_id;
                    //request for the command submission
                    std::vector<std::string> list_node_in_snapshot;
                    
                    std::string snapshot_name;
                    int64_t curren_node_id;
                    SnapshotCreationPhase phase;
                    
                    int storeDatasetTypeInSnapsnot(const std::string& job_work_code,
                                                   const std::string& snapshot_name,
                                                   const std::string& unique_id,
                                                   const std::string& dataset_type);
                    int storeDatasetForDeviceID(const std::string& device_id);
                public:
                    CreateSnapshotBatch();
                    ~CreateSnapshotBatch();
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

#endif /* __CHAOSFramework__88C5D13_DAD7_419F_ACBA_190123A754B0_CreateSnapshotBatch_h */

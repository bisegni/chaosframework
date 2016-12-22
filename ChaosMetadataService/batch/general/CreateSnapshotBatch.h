/*
 *	CreateSnapshotBatch.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 22/12/2016 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__88C5D13_DAD7_419F_ACBA_190123A754B0_CreateSnapshotBatch_h
#define __CHAOSFramework__88C5D13_DAD7_419F_ACBA_190123A754B0_CreateSnapshotBatch_h

#include "../mds_service_batch.h"

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace general {
                
                //!batch command for submit batch command within a node
                class CreateSnapshotBatch:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    //request for the command submission
                    std::vector<std::string> list_node_in_snapshot;
                    
                    std::string snapshot_name;
                    int64_t curren_node_id;
                    
                    int storeDatasetTypeInSnapsnot(const std::string& job_work_code,
                                                   const std::string& snapshot_name,
                                                   const std::string& unique_id,
                                                   const std::string& dataset_type);
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

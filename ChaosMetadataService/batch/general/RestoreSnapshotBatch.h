/*
 *	RestoreSnapshotBatch.hpp
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
                    std::auto_ptr<RequestInfo> restore_request;
                    std::auto_ptr<CDataWrapper> restore_message;
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

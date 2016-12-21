/*
 *	MongoDBSnapshotDataAccess.h
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

#ifndef __CHAOSFramework__MongoDBSnapshotDataAccess_h
#define __CHAOSFramework__MongoDBSnapshotDataAccess_h

#include "../data_access/SnapshotDataAccess.h"
#include "MongoDBNodeDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBSnapshotDataAccess:
                public data_access::SnapshotDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    MongoDBSnapshotDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
                                              data_access::DataServiceDataAccess *_data_service_da);
                    ~MongoDBSnapshotDataAccess();
                public:
                    //! Create a new snapshot
                    int snapshotCreateNewWithName(const std::string& snapshot_name,
                                                  std::string& working_job_unique_id);
                    
                    //! Add an element to a named snapshot
                    int snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
                                                     const std::string& snapshot_name,
                                                     const std::string& producer_unique_key,
                                                     const std::string& dataset_type,
                                                     void* data,
                                                     uint32_t data_len);
                    
                    int snapshotIncrementJobCounter(const std::string& working_job_unique_id,
                                                    const std::string& snapshot_name,
                                                    bool add);
                    
                    //! get the dataset from a snapshot
                    int snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
                                                         const std::string& producer_unique_key,
                                                         const std::string& dataset_type,
                                                         void **channel_data,
                                                         uint32_t& channel_data_size);
                    
                    //! Delete a snapshot where no job is working
                    int snapshotDeleteWithName(const std::string& snapshot_name);
                    
                    //! inherited method
                    int getNodeInSnapshot(const std::string& snapshot_name,
                                          ChaosStringVector& node_in_snapshot);
                    
                    //!inherited method
                    int getSnapshotForNode(const std::string& node_unique_id,
                                           ChaosStringVector& snapshot_for_node);
                    
                    //! inherited method
                    int getSnapshotWorkingState(const std::string& snapshot_name,
                                                bool& work_free);
                    
                    //! inherited method
                    int getAllSnapshot(chaos::metadata_service::persistence::data_access::SnapshotList& snapshot_desriptions);
                    
                    int getDatasetInSnapshotForNode(const std::string& node_unique_id,
                                                    const std::string& snapshot_name,
                                                    common::data::VectorStrCDWShrdPtr& snapshot_for_node);
                };
                
            }
        }
    }
}

#endif /* MongoDBSnapshotDataAccess_hpp */

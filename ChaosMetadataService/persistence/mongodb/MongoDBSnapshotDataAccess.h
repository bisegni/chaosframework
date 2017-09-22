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
                    MongoDBSnapshotDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
                                              data_access::DataServiceDataAccess *_data_service_da);
                    ~MongoDBSnapshotDataAccess();
                public:
                    //! Create a new snapshot
                    int snapshotCreateNewWithName(const std::string& snapshot_name,
                                                  std::string& working_job_unique_id);
                    
                    //! inherited method
                    int isSnapshotPresent(const std::string& snapshot_name,
                                          bool& presence);
                    
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
                    
                    //! inherited method
                    int getDatasetInSnapshotForNode(const std::string& node_unique_id,
                                                    const std::string& snapshot_name,
                                                    common::data::VectorStrCDWShrdPtr& snapshot_for_node);
                    
                    //! inherited method
                    int setDatasetInSnapshotForNode(const std::string& working_job_unique_id,
                                                    const std::string& node_unique_id,
                                                    const std::string& snapshot_name,
                                                    const std::string& dataset_key,
                                                    common::data::CDataWrapper& dataset_value);
                };
                
            }
        }
    }
}

#endif /* MongoDBSnapshotDataAccess_hpp */

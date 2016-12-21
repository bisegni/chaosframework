/*
 *	SnapshotDataAccess.h
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

#ifndef __CHAOSFramework__SnapshotDataAccess_h
#define __CHAOSFramework__SnapshotDataAccess_h

#include "../persistence.h"

#include <chaos/common/chaos_types.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class DataServiceDataAccess;
                
                typedef common::data::CDWShrdPtr                SnapshotElementPtr;
                typedef common::data::VectorCDWShrdPtr          SnapshotList;
                typedef common::data::VectorCDWShrdPtrIterator  SnapshotListIterator;
                class SnapshotDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                protected:
                    DataServiceDataAccess *data_service_da;
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    SnapshotDataAccess(DataServiceDataAccess *_data_service_da);
                    
                    //!default destructor
                    ~SnapshotDataAccess();
                    
                    //! Create a new snapshot
                    virtual int snapshotCreateNewWithName(const std::string& snapshot_name,
                                                          std::string& working_job_unique_id) = 0;
                    
                    //! Add an element to a named snapshot
                    virtual int snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
                                                             const std::string& snapshot_name,
                                                             const std::string& producer_unique_key,
                                                             const std::string& dataset_type,
                                                             void* data,
                                                             uint32_t data_len) = 0;
                    
                    virtual int snapshotIncrementJobCounter(const std::string& working_job_unique_id,
                                                            const std::string& snapshot_name,
                                                            bool add) = 0;
                    
                    //! get the dataset from a snapshot
                    virtual int snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
                                                                 const std::string& producer_unique_key,
                                                                 const std::string& dataset_type,
                                                                 void **channel_data,
                                                                 uint32_t& channel_data_size) = 0;
                    
                    //! Delete a snapshot where no job is working
                    virtual int snapshotDeleteWithName(const std::string& snapshot_name) = 0;
                    
                    //! get the node uid in a snapshot
                    /*!
                     \param snapshot_name the name of the snapshot
                     \param node_in_snapshot is the list of the node present in the snapshot
                     */
                    virtual int getNodeInSnapshot(const std::string& snapshot_name,
                                                  ChaosStringVector& node_in_snapshot) = 0;
                    
                    //!return all snapshot where node is present
                    virtual int getSnapshotForNode(const std::string& node_unique_id,
                                                   ChaosStringVector& snapshot_for_node) = 0;
                    
                    //!return all dataset in snapshot for a node
                    virtual int getDatasetInSnapshotForNode(const std::string& node_unique_id,
                                                            const std::string& snapshot_name,
                                                            common::data::VectorStrCDWShrdPtr& snapshot_for_node) = 0;
                    
                    //! check the snapshot work sate
                    /*!
                     The snapshot is created in an asynchronous way so this api return the state of
                     the work on snapshot
                     \param work_free is true is the snapshot is finished to be elaborated
                     */
                    virtual int getSnapshotWorkingState(const std::string& snapshot_name, bool& work_free) = 0;
                    
                    //! Return all shapshot
                    virtual int getAllSnapshot(SnapshotList& snapshot_desriptions) = 0;
                };
                
            }
        }
    }
}

#endif /* SnapshotDataAccess_hpp */

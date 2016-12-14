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
                    
                    //! Create a new snapshot for the node uid list
                    /*!
                     Perform in an asynchronous way a snapshot creation
                     \param snapshot_name the name of the new snapshot
                     \param node_uid_list the list of the node (identified by node unique id) to include into
                     snapshot.
                     */
                    virtual int createNewSnapshot(const std::string& snapshot_name,
                                                  const ChaosStringVector& node_uid_list);
                    
                    //! get the node uid in a snapshot
                    /*!
                     \param snapshot_name the name of the snapshot
                     \param node_in_snapshot is the list of the node present in the snapshot
                     */
                    virtual int getNodeInSnapshot(const std::string& snapshot_name,
                                                  ChaosStringVector& node_in_snapshot);
                    
                    //!return all snapshot where node is present
                    virtual int getSnapshotForNode(const std::string& node_unique_id,
                                                   ChaosStringVector& snapshot_for_node);
                    
                    //!return all dataset in snapshot for a node
                    virtual int getDatasetInSnapshotForNode(const std::string& node_unique_id,
                                                            const std::string& snapshot_name,
                                                            common::data::VectorStrCDWShrdPtr& snapshot_for_node) = 0;
                    
                    //! delete a snapshot
                    /*!
                     remove interelly the snapshot.
                     \param snapshot_name the name of the new snapshot
                     */
                    virtual int deleteSnapshot(const std::string& snapshot_name);
                    
                    //! check the snapshot work sate
                    /*!
                     The snapshot is created in an asynchronous way so this api return the state of
                     the work on snapshot
                     \param work_free is true is the snapshot is finished to be elaborated
                     */
                    virtual int getSnapshotWorkingState(const std::string& snapshot_name, bool& work_free) = 0;
                    
                    //! Return all shapshot
                    virtual int getAllSnapshot(SnapshotList& snapshot_desriptions) = 0;
                    
                    //! Create a new snapshot
                    /*!
                     Create a new snapshot with the name
                     \param snapshot_name the name of the new snapshot
                     \param the new job identification id, subseguent operation on snapshot need to be done using this code, otherwise
                     they will fails.
                     */
                    virtual int snapshotCreateNewWithName(const std::string& snapshot_name,
                                                          std::string& working_job_unique_id) = 0;
                    
                    //! Add an element to a named snapshot
                    /*!
                     add an element to the snapshot
                     \param working_job_unique_id the identification of the job
                     \param snapshot_name the name of the snapshot where put the element
                     \param producer_unique_key the unique key of the producer
                     \param dataset_type the type of the dataset, refer to @DataPackPrefixID field of the dataset
                     \param data the serialized data of the dataset
                     \param data_len the length of the serialized data
                     */
                    virtual int snapshotAddElementToSnapshot(const std::string& working_job_unique_id,
                                                             const std::string& snapshot_name,
                                                             const std::string& producer_unique_key,
                                                             const std::string& dataset_type,
                                                             void* data,
                                                             uint32_t data_len) = 0;
                    
                    //! Increment or decrement the number of the job that are working on
                    /*!
                     this function permit to manage the number of job that are working on the snapshot
                     \param working_job_unique_id the code associated to the job
                     \param snapshot_name the name of the snapshot to modify
                     \param add if true it add +1 if false add -1
                     */
                    virtual int snapshotIncrementJobCounter(const std::string& working_job_unique_id,
                                                            const std::string& snapshot_name,
                                                            bool add) = 0;
                    
                    //! get the dataset from a snapshot
                    /*!
                     Return the dataset asociated to a prducer key from a determinated
                     snapshot
                     \param snapshot_name the name of the snapshot to delete
                     \param producer_unique_key the unique key of the producer
                     \param dataset_type the type of the dataset, refer to @DataPackPrefixID field of the dataset
                     \param channel_data the data of the channel;
                     \param channel_data_size the size of the channel data
                     */
                    virtual int snapshotGetDatasetForProducerKey(const std::string& snapshot_name,
                                                                 const std::string& producer_unique_key,
                                                                 const std::string& dataset_type,
                                                                 void **channel_data,
                                                                 uint32_t& channel_data_size) = 0;
                    
                    //! Delete a snapshot where no job is working
                    /*!
                     Delete the snapshot and all dataset associated to it
                     \param snapshot_name the name of the snapshot to delete
                     */
                    virtual int snapshotDeleteWithName(const std::string& snapshot_name) = 0;
                };
                
            }
        }
    }
}

#endif /* SnapshotDataAccess_hpp */

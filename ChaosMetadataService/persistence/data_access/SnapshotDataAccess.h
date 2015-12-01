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

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class DataServiceDataAccess;
                
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
                                                  const std::vector<std::string> node_uid_list);
                    
                    //! get the node uid in a snapshot
                    /*!
                     \param snapshot_name the name of the snapshot
                     \param node_in_snapshot is the list of the node present in the snapshot
                     */
                    virtual int getNodeInSnapshot(const std::string& snapshot_name,
                                                  std::vector<std::string> node_in_snapshot);
                    
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
                };
                
            }
        }
    }
}

#endif /* SnapshotDataAccess_hpp */

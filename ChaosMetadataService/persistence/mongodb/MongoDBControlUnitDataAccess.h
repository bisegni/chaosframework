/*
 *	MongoDBControlUnitDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBControlUnitDataAccess__
#define __CHAOSFramework__MongoDBControlUnitDataAccess__


#include "MongoDBNodeDataAccess.h"
#include "../data_access/ControlUnitDataAccess.h"

#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <string>
#include <vector>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBControlUnitDataAccess:
                public data_access::ControlUnitDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBNodeDataAccess *node_data_access = NULL;
                protected:
                    MongoDBControlUnitDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBControlUnitDataAccess();
                public:

                    int checkPresence(const std::string& control_unit_id, bool& presence);

                    int insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description);
                    
                    int updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description);
                    
                    int setDataset(chaos::common::data::CDataWrapper& dataset_description);
                    
                    int checkDatasetPresence(chaos::common::data::CDataWrapper& dataset_description);
                    
                    int getLastDataset(chaos::common::data::CDataWrapper& dataset_description);

                    int setInstanceDescription(const std::string& cu_unique_id,
                                               chaos::common::data::CDataWrapper& instance_description);

                    int searchInstanceForUnitServer(std::vector<boost::shared_ptr<chaos::common::data::CDataWrapper> >& result_page,
                                                    const std::string& unit_server_uid,
                                                    std::vector<std::string> cu_type_filter,
                                                    uint32_t last_sequence_id,
                                                    uint32_t results_for_page);

                    int getInstanceDescription(const std::string& unit_server_uid,
                                               const std::string& control_unit_uid,
                                               chaos::common::data::CDataWrapper **result);

                    int getInstanceDescription(const std::string& control_unit_uid,
                                               chaos::common::data::CDataWrapper **result);

                    int deleteInstanceDescription(const std::string& unit_server_uid,
                                                  const std::string& control_unit_uid);
                };

                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBControlUnitDataAccess__) */

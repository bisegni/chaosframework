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
                    
                    MongoDBNodeDataAccess *node_data_access;
                protected:
                    MongoDBControlUnitDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
                                                 data_access::DataServiceDataAccess *_data_service_da);
                    ~MongoDBControlUnitDataAccess();
                    
                    int compeleteControlUnitForAgeingManagement(const std::string& control_unit_id);
                public:
                    
                    int checkPresence(const std::string& control_unit_id, bool& presence);
                    
                    int getControlUnitWithAutoFlag(const std::string& unit_server_host,
                                                   chaos::metadata_service::persistence::AutoFlag auto_flag,
                                                   uint64_t last_sequence_id,
                                                   std::vector<NodeSearchIndex>& control_unit_found);
                    
                    int insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description);
                    
                    int updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description);
                    
                    int setDataset(const std::string& cu_unique_id,
                                   chaos::common::data::CDataWrapper& dataset_description);
                    
                    int checkDatasetPresence(const std::string& cu_unique_id,
                                             bool& presence);
                    
                    int getDataset(const std::string& cu_unique_id,
                                   chaos::common::data::CDataWrapper **dataset_description);
                    
                    int setCommandDescription(chaos::common::data::CDataWrapper& command_description);
                    
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
                    
                    int getInstanceDatasetAttributeDescription(const std::string& control_unit_uid,
                                                               const std::string& attribute_name,
                                                               boost::shared_ptr<chaos::common::data::CDataWrapper>& result);
                    
                    int getInstanceDatasetAttributeConfiguration(const std::string& control_unit_uid,
                                                                 const std::string& attribute_name,
                                                                 boost::shared_ptr<chaos::common::data::CDataWrapper>& result);
                    
                    int getDataServiceAssociated(const std::string& cu_uid,
                                                 std::vector<std::string>& associated_ds);
                    
                    int reserveControlUnitForAgeingManagement(uint64_t& last_sequence_id,
                                                              std::string& control_unit_found,
                                                              uint32_t& control_unit_ageing_time,
                                                              uint64_t& last_ageing_perform_time,
                                                              uint64_t timeout_for_checking = 30000,
                                                              uint64_t delay_next_check = 3600000);
                    
                    int releaseControlUnitForAgeingManagement(std::string& control_unit_found,
                                                              bool performed);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBControlUnitDataAccess__) */

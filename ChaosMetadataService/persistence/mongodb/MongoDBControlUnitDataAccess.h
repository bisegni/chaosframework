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
                    MongoDBControlUnitDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection,
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
                    
                    int getDataset(const std::string& cu_unique_id,
                                   chaos::common::data::structured::Dataset& dataset);
                    
                    int getFullDescription(const std::string& cu_unique_id,
                                           chaos::common::data::CDataWrapper **dataset_description);
                    
                    int setCommandDescription(chaos::common::data::CDataWrapper& command_description);
                    
                    int setInstanceDescription(const std::string& cu_unique_id,
                                               chaos::common::data::CDataWrapper& instance_description);
                    
                    int searchInstanceForUnitServer(std::vector<ChaosSharedPtr<chaos::common::data::CDataWrapper> >& result_page,
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
                                                               ChaosSharedPtr<chaos::common::data::CDataWrapper>& result);
                    
                    int getInstanceDatasetAttributeConfiguration(const std::string& control_unit_uid,
                                                                 const std::string& attribute_name,
                                                                 ChaosSharedPtr<chaos::common::data::CDataWrapper>& result);
                    
                    int getScriptAssociatedToControlUnitInstance(const std::string& cu_instance,
                                                                 bool& found,
                                                                 chaos::service_common::data::script::ScriptBaseDescription& script_base_descrition);
                    
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
                    
                    int eraseControlUnitDataBeforeTS(const std::string& control_unit_id,
                                                     uint64_t unit_ts);
                    
                    int getNextRunID(const std::string& control_unit_id, int64_t& run_id);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBControlUnitDataAccess__) */

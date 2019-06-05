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

#ifndef __CHAOSFramework_BC38331E_2F7E_4033_B2EA_C03D6D694CD7_MongoDBScriptDataAccess_h
#define __CHAOSFramework_BC38331E_2F7E_4033_B2EA_C03D6D694CD7_MongoDBScriptDataAccess_h

#include "../data_access/ScriptDataAccess.h"
#include "MongoDBUtilityDataAccess.h"
#include "MongoDBNodeDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for manage the log
                /*!
                 */
                class MongoDBScriptDataAccess:
                public data_access::ScriptDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBUtilityDataAccess *utility_data_access = NULL;
                    MongoDBNodeDataAccess *node_data_access = NULL;
                    //return the query for a page
                    mongo::Query getNextPagedQuery(uint64_t last_sequence_before_this_page,
                                                   const std::string& search_string);
                    
                    mongo::Query getNextPagedQueryForInstance(uint64_t last_sequence_before_this_page,
                                                              const std::string& script_name,
                                                              const std::string& search_string);
                    
                protected:
                    MongoDBScriptDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBScriptDataAccess();
                public:
                    int insertNewScript(ChaosUniquePtr<chaos::common::data::CDataWrapper>& new_Script);
                    int updateScript(ChaosUniquePtr<chaos::common::data::CDataWrapper>& script);
                    int searchScript(ChaosUniquePtr<chaos::common::data::CDataWrapper>& script_list,
                                             const std::string& search_string,
                                             uint64_t start_sequence_id,
                                             uint32_t page_length);
                    int loadScript(const uint64_t unique_id,
                                           const std::string& name,
                                           ChaosUniquePtr<chaos::common::data::CDataWrapper>& script,
                                           bool load_source_code = false);
                 
                    //! Inherited method
                    int insertNewScript(chaos::service_common::data::script::Script& new_Script);
                    
                    //! Inherited method
                    int updateScript(chaos::service_common::data::script::Script& script);
                    
                    //! Inherited Method
                    int searchScript(chaos::service_common::data::script::ScriptBaseDescriptionListWrapper& script_list,
                                     const std::string& search_string,
                                     uint64_t last_sequence_id,
                                     uint32_t page_length);
                    
                    //! Inherited Method
                    int addScriptInstance(const uint64_t seq,
                                          const std::string& script_name,
                                          const std::string& instance_name);
                    
                    //! Inherited Method
                    int updateBindType(const chaos::service_common::data::script::ScriptBaseDescription& script_base_descrition,
                                       const chaos::service_common::data::script::ScriptInstance& instance);
                    
                    //! Inherited Method
                    int removeScriptInstance(const uint64_t seq,
                                             const std::string& script_name,
                                             const std::string& instance_name);
                    
                    //! Inherited Method
                    int searchScriptInstance(std::vector<chaos::service_common::data::script::ScriptInstance>& instance_list,
                                             const std::string& script_name,
                                             const std::string& search_string,
                                             uint64_t start_sequence_id,
                                             uint32_t page_length);
                    
                    // Inherited Method
                    int getScriptInstance(const std::string& script_instance,
                                          const uint64_t& seq,
                                          bool& found,
                                          chaos::service_common::data::script::ScriptInstance& instance);
                    
                    //! Inherited Method
                    int loadScript(const uint64_t unique_id,
                                   const std::string& name,
                                   chaos::service_common::data::script::Script& script,
                                   bool load_source_code = false);
                    
                    //! Inherited Method
                    int deleteScript(const uint64_t unique_id,
                                     const std::string& name);
                    
                    //! Inherited Method
                    int getScriptForExecutionPoolPathList(const ChaosStringVector& pool_path,
                                                          std::vector<chaos::service_common::data::script::ScriptBaseDescription>& script_found,
                                                          uint64_t last_sequence_id = 0,
                                                          uint32_t max_result = 10);
                    
                    //! Inherited Method
                    int getUnscheduledInstanceForJob(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                     ChaosStringVector& instance_found_list,
                                                     uint32_t timeout = 30000,
                                                     uint32_t max_result = 10);
                    
                    //! Inherited Method
                    int reserveInstanceForScheduling(bool& reserverd,
                                                     const std::string& instance_uid,
                                                     const std::string& unit_server_parent,
                                                     uint32_t timeout = 30000);
                    
                    //! Inherited Method
                    int copyScriptDatasetAndContentToInstance(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                              const std::string& script_instance);
                    
                    //! Inherited Method
                    int instanceForUnitServerHeartbeat(const ChaosStringVector& script_instance_list,
                                                       const std::string& unit_server_parent,
                                                       uint32_t timeout);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_BC38331E_2F7E_4033_B2EA_C03D6D694CD7_MongoDBScriptDataAccess_h */

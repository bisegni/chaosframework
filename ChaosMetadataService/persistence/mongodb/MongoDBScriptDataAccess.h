/*
 *	MongoDBScriptDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/05/16 INFN, National Institute of Nuclear Physics
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
                    MongoDBScriptDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBScriptDataAccess();
                public:
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
                    int removeScriptInstance(const uint64_t seq,
                                             const std::string& script_name,
                                             const std::string& instance_name);
                    
                    //! Inherited Method
                    int searchScriptInstance(std::vector<chaos::service_common::data::node::NodeInstance>& instance_list,
                                             const std::string& script_name,
                                             const std::string& search_string,
                                             uint64_t start_sequence_id,
                                             uint32_t page_length);
                    //! Inherited Method
                    int loadScript(const uint64_t unique_id,
                                   const std::string& name,
                                   chaos::service_common::data::script::Script& script,
                                   bool load_source_code = false);
                    
                    //! Inherited Method
                    int getScriptForExecutionPoolPathList(const ChaosStringVector& pool_path,
                                                          std::vector<chaos::service_common::data::script::ScriptBaseDescription>& script_found,
                                                          uint32_t max_result = 10);
                    
                    //! Inherited Method
                    int getUnscheduledInstanceForJob(const chaos::service_common::data::script::ScriptBaseDescription& script,
                                                     ChaosStringVector& instance_found_list,
                                                     uint32_t timeout = 30000,
                                                     uint32_t max_result = 10);
                    
                    //! Inherited Method
                    int reserveInstanceForScheduling(const std::string& instance_uid,
                                                     const std::string& unit_server_parent,
                                                     uint32_t timeout = 30000);
                    
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

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
#ifndef __CHAOSFramework__MongoDBNodeDataAccess__
#define __CHAOSFramework__MongoDBNodeDataAccess__
#include "MongoDBUtilityDataAccess.h"
#include "../data_access/NodeDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBNodeDataAccess:
                public data_access::NodeDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBUtilityDataAccess *utility_data_access;
                    
                protected:
                    MongoDBNodeDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBNodeDataAccess();
                    
                    mongo::BSONObj getAliveOption(unsigned int timeout_sec);
                public:
                    //inherited method
                    int getNodeDescription(const std::string& node_unique_id,
                                           chaos::common::data::CDataWrapper **node_description);
                    //inherited method
                    int insertNewNode(chaos::common::data::CDataWrapper& node_description);
                    //! update the node updatable feature
                    int updateNode(chaos::common::data::CDataWrapper& node_description);
                    // inherited method
                    int checkNodePresence(bool& presence,
                                          const std::string& node_unique_id,
                                          const std::string& node_unique_type = std::string());
                    // inherited method
                    int setNodeHealthStatus(const std::string& node_unique_id,
                                            const common::data::structured::HealthStat& health_stat);
                    
                    // inherited methods
                    int getNodeHealthStatus(const std::string& node_unique_id,
                                            common::data::structured::HealthStat& health_stat);
                    
                    //! inherited method
                    int deleteNode(const std::string& node_unique_id,
                                   const std::string& node_type = std::string());
                    //! inherited method
                    int searchNode(chaos::common::data::CDataWrapper **result,
                                   const std::string& criteria,
                                   uint32_t search_type,
                                   bool alive_only,
                                   uint32_t last_unique_id,
                                   uint32_t page_length);
                    //! inherited method
                    int setProperty(const std::string& node_uid,
                                    const chaos::common::property::PropertyGroupVector& property_group_vector);
                    //! inherited method
                    int updatePropertyDefaultValue(const std::string& node_uid,
                                                   const chaos::common::property::PropertyGroupVector& property_group_vector);
                    //! inherited method
                    int getProperty(const data_access::PropertyType property_type,
                                    const std::string& node_uid,
                                    chaos::common::property::PropertyGroupVector& property_group_vector);
                    //! inherited method
                    int getPropertyGroup(const data_access::PropertyType property_type,
                                         const std::string& node_uid,
                                         const std::string& property_group_name,
                                         chaos::common::property::PropertyGroup& property_group);
                    //! inherited method
                    int getCommand(const std::string& command_unique_id,
                                   chaos::common::data::CDataWrapper **command);
                    //! inherited method
                    int checkCommandPresence(const std::string& command_unique_id,
                                             bool& presence);
                    //! inherited method
                    int setCommand(chaos::common::data::CDataWrapper& command);
                    
                    //! inherited method
                    int deleteCommand(const std::string& command_unique_id);
                    
                    //! inherited method
                    int checkCommandTemplatePresence(const std::string& template_name,
                                                     const std::string& command_unique_id,
                                                     bool& presence);
                    //! inherited method
                    int setCommandTemplate(chaos::common::data::CDataWrapper& command_template);
                    //! inherited method
                    int deleteCommandTemplate(const std::string& template_name,
                                              const std::string& command_unique_id);
                    //! inherited method
                    int getCommandTemplate(const std::string& template_name,
                                           const std::string& command_unique_id,
                                           chaos::common::data::CDataWrapper **command_template);
                    //! inherited method
                    int searchCommandTemplate(chaos::common::data::CDataWrapper **result,
                                              const std::vector<std::string>& cmd_uid_to_filter,
                                              uint32_t last_unique_id,
                                              uint32_t page_length = 100);
                    //! inherited method
                    int addAgeingManagementDataToNode(const std::string& control_unit_id);
                    
                    //                    int reserveNodeForAgeingManagement(uint64_t& last_sequence_id,
                    //                                                       std::string& node_uid_reserved,
                    //                                                       uint32_t& node_ageing_time,
                    //                                                       uint64_t& last_ageing_perform_time,
                    //                                                       uint64_t timeout_for_checking,
                    //                                                       uint64_t delay_next_check);
                    //
                    //                    int releaseNodeForAgeingManagement(std::string& node_uid,
                    //                                                       bool performed);
                    
                    int isNodeAlive(const std::string& node_uid, bool& alive);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBUnitServerDataAccess__) */

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

#ifndef __CHAOSFramework__MongoDBDataServiceDataAccess__
#define __CHAOSFramework__MongoDBDataServiceDataAccess__

#include "../data_access/DataServiceDataAccess.h"
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
                class MongoDBDataServiceDataAccess:
                public data_access::DataServiceDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    MongoDBNodeDataAccess *node_data_access = NULL;
                protected:
                    MongoDBDataServiceDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBDataServiceDataAccess();
                public:
                    //inherited method
                    int checkPresence(const std::string& ds_unique_id,
                                      bool& presence);
                    
                    int getDescription(const std::string& ds_unique_id,
                                       chaos::common::data::CDataWrapper **node_description);
                    
                    //inherited method
                    int registerNode(const std::string& ds_zone,
                                     const std::string& ds_unique_id,
                                     const std::string& ds_direct_io_addr,
                                     uint32_t endpoint);
                    
                    //inherited method
                    int updateNodeStatistic(const std::string& ds_unique_id,
                                            const std::string& ds_direct_io_addr,
                                            const uint32_t endpoint,
                                            const chaos::common::utility::ProcStat& process_resuorce_usage,const std::string& zone);
                    
                    //inherited method
                    int deleteDataService(const std::string& ds_unique_id);
                    
                    //inherited method
                    int associateNode(const std::string& ds_unique_id,
                                      const std::string& associated_node_unique_id);
                    
                    //inherited method
                    int removeNode(const std::string& ds_unique_id,
                                   const std::string& associated_node_unique_id);
                    
                    //inherited method
                    int searchAssociationForUID(const std::string& ds_unique_id,
                                                std::vector<ChaosSharedPtr<common::data::CDataWrapper> >& node_associated);
                    //inherited method
                    int searchAllDataAccess(std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  node_associated,
                                            uint32_t last_unique_id,
                                            uint32_t page_length);
                    //inherited method
                    int getBestNDataService(const std::string& ha_zone,
                                            std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  best_available_data_service,
                                            unsigned int number_of_result = 3);
                    //inherited method
                    int getBestNDataService(const std::string& ha_zone,
                                            std::vector<std::string >&  data_service_uid_list,
                                            unsigned int number_of_result = 3);
                    //inherited method
                    int getBestNDataServiceEndpoint(const std::string& ha_zone,
                                                    std::vector<std::string >&  data_service_endpoint_list,
                                                    unsigned int number_of_result =3);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBDataServiceDataAccess__) */

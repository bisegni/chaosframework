/*
 *	MongoDBDataServiceDataAccess.h
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
                    MongoDBDataServiceDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBDataServiceDataAccess();
                public:
                        //inherited method
                    int checkPresence(const std::string& ds_unique_id,
                                      bool& presence);

                        //inherited method
                    int insertNew(const std::string& ds_unique_id,
                                  const std::string& ds_direct_io_addr,
                                  uint32_t endpoint);

                        //inherited method
                    int updateExisting(const std::string& ds_unique_id,
                                       const std::string& ds_direct_io_addr,
                                       uint32_t endpoint);

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
                                                std::vector<boost::shared_ptr<common::data::CDataWrapper> >& node_associated);

                    int searchAllDataAccess(std::vector<boost::shared_ptr<common::data::CDataWrapper> >&  node_associated,
                                            uint32_t last_unique_id,
                                            uint32_t page_length);
                };


            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBDataServiceDataAccess__) */

/*
 *	DataServiceDataAccess.h
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

#ifndef __CHAOSFramework__DataServiceDataAccess__
#define __CHAOSFramework__DataServiceDataAccess__

#include "../persistence.h"
#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {

                class DataServiceDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess{
                public:
                    DECLARE_DA_NAME

                        //! default constructor
                    DataServiceDataAccess();

                        //!default destructor
                    ~DataServiceDataAccess();

                    virtual int checkPresence(const std::string& ds_unique_id,
                                              bool& presence) = 0;

                    virtual int insertNew(const std::string& ds_unique_id,
                                          const std::string& ds_direct_io_addr,
                                          uint32_t endpoint) = 0;

                    virtual int getDescription(const std::string& ds_unique_id,
                                               chaos::common::data::CDataWrapper **node_description) = 0;

                    virtual int updateExisting(const std::string& ds_unique_id,
                                               const std::string& ds_direct_io_addr,
                                               uint32_t endpoint) = 0;

                    virtual int deleteDataService(const std::string& ds_unique_id) = 0;

                    virtual int associateNode(const std::string& ds_unique_id,
                                              const std::string& associated_node_unique_id) = 0;

                    virtual int removeNode(const std::string& ds_unique_id,
                                           const std::string& associated_node_unique_id) = 0;

                    virtual int searchAssociationForUID(const std::string& ds_unique_id,
                                                        std::vector<boost::shared_ptr<common::data::CDataWrapper> >&  node_associated) = 0;

                    virtual int searchAllDataAccess(std::vector<boost::shared_ptr<common::data::CDataWrapper> >&  node_associated,
                                                    uint32_t last_unique_id,
                                                    uint32_t page_length) = 0;

                    virtual int getBestNDataService(std::vector<boost::shared_ptr<common::data::CDataWrapper> >&  best_available_data_service,
                                                    unsigned int numerb_of_result = 3) = 0;
                    
                    virtual int getBestNDataService(std::vector<std::string >&  best_available_data_service,
                                                    unsigned int numerb_of_result = 3) = 0;
                    
                    virtual int getBestNDataServiceEndpoint(std::vector<std::string >&  best_available_data_service_endpoint,
                                                            unsigned int numerb_of_result = 3) = 0;
                };

            }
        }
    }
}


#endif /* defined(__CHAOSFramework__DataServiceDataAccess__) */

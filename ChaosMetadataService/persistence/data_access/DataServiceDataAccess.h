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

#ifndef __CHAOSFramework__DataServiceDataAccess__
#define __CHAOSFramework__DataServiceDataAccess__

#include "../persistence.h"

#include <chaos/common/utility/ProcStat.h>

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
                    
                    virtual int getDescription(const std::string& ds_unique_id,
                                               chaos::common::data::CDataWrapper **node_description) = 0;
                    
                    virtual int registerNode(const std::string& ds_zone,
                                             const std::string& ds_unique_id,
                                             const std::string& ds_direct_io_addr,
                                             uint32_t endpoint) = 0;
                    
                    virtual int deleteDataService(const std::string& ds_unique_id) = 0;
                    
                    virtual int associateNode(const std::string& ds_unique_id,
                                              const std::string& associated_node_unique_id) = 0;
                    
                    virtual int removeNode(const std::string& ds_unique_id,
                                           const std::string& associated_node_unique_id) = 0;
                    
                    virtual int searchAssociationForUID(const std::string& ds_unique_id,
                                                        std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  node_associated) = 0;
                    
                    virtual int searchAllDataAccess(std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  node_associated,
                                                    uint32_t last_unique_id,
                                                    uint32_t page_length) = 0;
                    
                    virtual int getBestNDataService(const std::string& ds_zone,
                                                    std::vector<ChaosSharedPtr<common::data::CDataWrapper> >&  best_available_data_service,
                                                    unsigned int numerb_of_result = 3) = 0;
                    
                    virtual int getBestNDataService(const std::string& ds_zone,
                                                    std::vector<std::string >&  best_available_data_service,
                                                    unsigned int numerb_of_result = 3) = 0;
                    
                    virtual int getBestNDataServiceEndpoint(const std::string& ds_zone,
                                                            std::vector<std::string >&  best_available_data_service_endpoint,
                                                            unsigned int numerb_of_result = 3) = 0;
                    
                    
                };
                
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__DataServiceDataAccess__) */

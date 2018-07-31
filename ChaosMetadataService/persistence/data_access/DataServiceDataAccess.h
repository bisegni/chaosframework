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

                /**
                 * @brief check if the data accessi is present in database
                 * 
                 * @param ds_unique_id 
                 * @param presence 
                 * @return int api error
                 */
                virtual int checkPresence(const std::string& ds_unique_id,
                                          bool&              presence) = 0;

                /**
                 * @brief Get the Description object od the data service node
                 * 
                 * @param ds_unique_id 
                 * @param node_description 
                 * @return int api error
                 */
                virtual int getDescription(const std::string&                  ds_unique_id,
                                           chaos::common::data::CDataWrapper** node_description) = 0;

                /**
                 * @brief Register a new dataaccess node
                 * 
                 * @param ds_zone 
                 * @param ds_unique_id 
                 * @param ds_direct_io_addr 
                 * @param endpoint 
                 * @return int api error
                 */
                virtual int registerNode(const std::string& ds_zone,
                                         const std::string& ds_unique_id,
                                         const std::string& ds_direct_io_addr,
                                         uint32_t           endpoint) = 0;
                
                /**
                 * @brief update the node statistic
                 * 
                 * @param ds_unique_id 
                 * @param ds_direct_io_addr 
                 * @param endpoint 
                 * @param process_resuorce_usage 
                 * @param zone 
                 * @return int api error
                 */
                virtual int updateNodeStatistic(const std::string&                      ds_unique_id,
                                                const std::string&                      ds_direct_io_addr,
                                                const uint32_t                          endpoint,
                                                const chaos::common::utility::ProcStat& process_resuorce_usage,
                                                const std::string&                      zone) = 0;

                /**
                 * @brief delete a data service
                 * 
                 * @param ds_unique_id 
                 * @return int api error
                 */
                virtual int deleteDataService(const std::string& ds_unique_id) = 0;

                /**
                 * @brief associate a node to a data service
                 * 
                 * @param ds_unique_id node id of the data service
                 * @param associated_node_unique_id id to assocaite to the data access
                 * @return int api error
                 */
                virtual int associateNode(const std::string& ds_unique_id,
                                          const std::string& associated_node_unique_id) = 0;

                /**
                 * @brief remove a node already associated to a data servicec
                 * 
                 * @param ds_unique_id data access id
                 * @param associated_node_unique_id node to be removed form the data access
                 * @return int api error
                 */
                virtual int removeNode(const std::string& ds_unique_id,
                                       const std::string& associated_node_unique_id) = 0;
                
                /**
                 * @brief get all assocaited node to a data service 
                 * 
                 * @param ds_unique_id data service uid
                 * @param node_associated assocaited node uid
                 * @return int api error
                 */
                virtual int searchAssociationForUID(const std::string&                                        ds_unique_id,
                                                    std::vector<ChaosSharedPtr<common::data::CDataWrapper> >& node_associated) = 0;

                /**
                 * @brief find on all data service
                 * 
                 * @param found_description all descirpiton found in current page
                 * @param last_unique_id last found unique id
                 * @param page_length the lenght of current page
                 * @return int  api error
                 */
                virtual int searchAllDataAccess(std::vector<ChaosSharedPtr<common::data::CDataWrapper> >& found_description,
                                                uint32_t                                                  last_unique_id,
                                                uint32_t                                                  page_length) = 0;

                /**
                 * @brief Get the Best N Data Service object, the best dataservice
                 * all got from those have a good metrics (minus cpu usage and other resources)
                 * 
                 * @param ds_zone the zone form wich we want the data service need to belong
                 * @param best_available_data_service found data service description
                 * @param numerb_of_result number of entries in the result
                 * @param filter_out_dio_addr exclude from the search the data service assocaited to the 
                 *                  id in the list
                 * @return int api error
                 */
                virtual int getBestNDataService(const std::string&                                        ds_zone,
                                                std::vector<ChaosSharedPtr<common::data::CDataWrapper> >& best_available_data_service,
                                                unsigned int                                              numerb_of_result = 3) = 0;

                /**
                 * @brief Get the Best N Data Service object, the best dataservice
                 * all got from those have a good metrics (minus cpu usage and other resources)
                 * 
                 * @param ds_zone the zone form wich we want the data service need to belong
                 * @param data_service_uid_list the node uid list of the found data service
                 * @param numerb_of_result number for entries in the result
                 * @param filter_out_dio_addr exclude from the search the data service assocaited to the 
                 *                  id in the list
                 * @return int api error
                 */
                virtual int getBestNDataService(const std::string&    ds_zone,
                                                ChaosStringVector&    data_service_uid_list,
                                                unsigned int          numerb_of_result = 3) = 0;

                /** @brief Get the Best N Data Service object, the best dataservice
                 * all got from those have a good metrics (minus cpu usage and other resources)
                 * 
                 * @param ds_zone the zone form wich we want the data service need to belong
                 * @param data_service_endpoint_list the node uid list of the found data service
                 * @param numerb_of_result number for entries in the result
                 * @param filter_out_dio_addr exclude from the search the data service assocaited to the 
                 *                  id in the list
                 * @return int api error
                 */
                virtual int getBestNDataServiceEndpoint(const std::string&    ds_zone,
                                                        ChaosStringVector&    data_service_endpoint_list,
                                                        unsigned int          numerb_of_result = 3) = 0;
                };
                
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__DataServiceDataAccess__) */

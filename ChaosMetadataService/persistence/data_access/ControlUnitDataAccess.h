/*
 *	ControlUnitDataAccess.h
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
#ifndef __CHAOSFramework__ControlUnitDataAccess__
#define __CHAOSFramework__ControlUnitDataAccess__

#include "../persistence.h"
#include "../persistence_types.h"

#include <chaos/common/data/CDataWrapper.h>

#include <boost/smart_ptr.hpp>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                class DataServiceDataAccess;
                
                class ControlUnitDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                protected:
                    DataServiceDataAccess *data_service_da;
                public:
                    DECLARE_DA_NAME
                    //! default constructor
                    ControlUnitDataAccess(DataServiceDataAccess *_data_service_da);
                    
                    //!default destructor
                    ~ControlUnitDataAccess();
                    
                    //!check if the control unit node is present
                    virtual int checkPresence(const std::string& control_unit_id, bool& presence) = 0;
                    
                    virtual int getControlUnitWithAutoFlag(const std::string& unit_server_host,
                                                           chaos::metadata_service::persistence::AutoFlag auto_flag,
                                                           uint64_t last_sequence_id,
                                                           std::vector<NodeSearchIndex>& control_unit_found) = 0;
                    
                    virtual int insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description) = 0;
                    
                    virtual int updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description) = 0;
                    
                    virtual int setDataset(const std::string& cu_unique_id,
                                           chaos::common::data::CDataWrapper& dataset_description) = 0;
                    
                    virtual int checkDatasetPresence(const std::string& cu_unique_id,
                                                     bool& presence) = 0;
                    
                    virtual int getDataset(const std::string& cu_unique_id,
                                           chaos::common::data::CDataWrapper **dataset_description) = 0;
                    
                    virtual int setCommandDescription(chaos::common::data::CDataWrapper& command_description) = 0;
                    
                    virtual int setInstanceDescription(const std::string& cu_unique_id,
                                                       chaos::common::data::CDataWrapper& instance_description) = 0;
                    
                    virtual int searchInstanceForUnitServer(std::vector<boost::shared_ptr<common::data::CDataWrapper> >& result_page,
                                                            const std::string& unit_server_uid,
                                                            std::vector<std::string> cu_type_filter,
                                                            uint32_t last_sequence_id,
                                                            uint32_t results_for_page) = 0;
                    
                    virtual int getInstanceDescription(const std::string& unit_server_uid,
                                                       const std::string& control_unit_uid,
                                                       chaos::common::data::CDataWrapper **result) = 0;
                    
                    virtual int getInstanceDescription(const std::string& control_unit_uid,
                                                       chaos::common::data::CDataWrapper **result) = 0;
                    
                    virtual int deleteInstanceDescription(const std::string& unit_server_uid,
                                                          const std::string& control_unit_uid) = 0;
                    
                    virtual int getInstanceDatasetAttributeDescription(const std::string& control_unit_uid,
                                                                       const std::string& attribute_name,
                                                                       boost::shared_ptr<chaos::common::data::CDataWrapper>& result) = 0;
                    
                    virtual int getInstanceDatasetAttributeConfiguration(const std::string& control_unit_uid,
                                                                         const std::string& attribute_name,
                                                                         boost::shared_ptr<chaos::common::data::CDataWrapper>& result) = 0;
                    
                    //! return the data service associater to control unit
                    virtual int getDataServiceAssociated(const std::string& cu_uid,
                                                         std::vector<std::string>& associated_ds) = 0;
                    //!return the oldest checkd control unit that has ageing expiration time
                    /*!
                     \param last_device_id is an input out parameter, in input it represent last seq id found,
                            in output it is valorized with the sequence number of found control unit(is equal
                            to 0 if no control uni thas been found
                     \param control_unit_found uinique id of found control unit, if empty control uni thas not been found
                     \param last_ageing_check_time is the found control unit last time that ageing has been checked
                     */
                    virtual int reserveControlUnitForAgeingManagement(uint64_t& last_sequence_id,
                                                                      std::string& control_unit_found,
                                                                      uint32_t& control_unit_ageing_time,
                                                                      uint64_t& last_ageing_perform_time) = 0;
                    
                    virtual int releaseControlUnitForAgeingManagement(std::string& control_unit_found,
                                                                      bool performed) = 0;
                    
                    //!Remove all data befor the unit_ts timestamp
                    /*!
                     \param control_unit_id the unique id of the control unit
                     \param unit_ts is the timestamp befor wich we want to erase all data(included it)
                     */
                    virtual int eraseControlUnitDataBeforeTS(const std::string& control_unit_id,
                                                             uint64_t unit_ts);
                };
                
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__ControlUnitDataAccess__) */

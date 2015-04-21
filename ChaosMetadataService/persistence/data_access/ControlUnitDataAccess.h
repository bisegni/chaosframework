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

#include <chaos/common/data/CDatawrapper.h>

#include <boost/smart_ptr.hpp>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class ControlUnitDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                public:
                    DECLARE_DA_NAME

                    //! default constructor
                    ControlUnitDataAccess();
                    
                    //!default destructor
                    ~ControlUnitDataAccess();

                        //!check if the control unit node is present
                    virtual int checkPresence(const std::string& control_unit_id, bool& presence) = 0;

                    virtual int insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description) = 0;
                    
                    virtual int updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description) = 0;
                    
                    virtual int setDataset(chaos::common::data::CDataWrapper& dataset_description) = 0;
                    
                    virtual int checkDatasetPresence(chaos::common::data::CDataWrapper& dataset_description) = 0;
                    
                    virtual int getLastDataset(chaos::common::data::CDataWrapper& dataset_description) = 0;

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
                };
                
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__ControlUnitDataAccess__) */

/*
 *	ControlUnitDataAccess.h
 *	!CHOAS
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

#include "../AbstractDataAccess.h"
#include <chaos/common/data/CDatawrapper.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                
                class ControlUnitDataAccess:
                public AbstractDataAccess {
                public:
                    //! default constructor
                    ControlUnitDataAccess();
                    
                    //!default destructor
                    ~ControlUnitDataAccess();
                    
                    virtual int insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description) = 0;
                    
                    virtual int updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description) = 0;
                    
                    virtual int addNewDataset(chaos::common::data::CDataWrapper& dataset_description) = 0;
                    
                    virtual int checkDatasetPresence(chaos::common::data::CDataWrapper& dataset_description) = 0;
                    
                    virtual int getLastDataset(chaos::common::data::CDataWrapper& dataset_description) = 0;
                };
                
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__ControlUnitDataAccess__) */

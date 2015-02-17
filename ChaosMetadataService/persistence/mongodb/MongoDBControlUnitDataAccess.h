/*
 *	MongoDBControlUnitDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBControlUnitDataAccess__
#define __CHAOSFramework__MongoDBControlUnitDataAccess__


#include "MongoDBAccessor.h"
#include "MongoDBNodeDataAccess.h"
#include "../data_access/ControlUnitDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBControlUnitDataAccess:
                public data_access::ControlUnitDataAccess,
                protected MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    friend class INSTANCER_P1(MongoDBControlUnitDataAccess, AbstractDataAccess, const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>&);
                    
                    MongoDBNodeDataAccess *node_data_access = NULL;
                protected:
                    MongoDBControlUnitDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBControlUnitDataAccess();
                public:
                    int insertNewControlUnit(chaos::common::data::CDataWrapper& control_unit_description);
                    
                    int updateControlUnit(chaos::common::data::CDataWrapper& control_unit_description);
                    
                    int addNewDataset(chaos::common::data::CDataWrapper& dataset_description);
                    
                    int checkDatasetPresence(chaos::common::data::CDataWrapper& dataset_description);
                    
                    int getLastDataset(chaos::common::data::CDataWrapper& dataset_description);
                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBControlUnitDataAccess__) */

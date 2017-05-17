/*
 *	MongoDBDeployDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/03/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EA932A6F_4000_47DE_9A9D_DA37C4AEC5DF_DeployDataAccess_h
#define __CHAOSFramework_EA932A6F_4000_47DE_9A9D_DA37C4AEC5DF_DeployDataAccess_h

#include "../data_access/DeployDataAccess.h"
#include "MongoDBUtilityDataAccess.h"
#include "MongoDBNodeDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for manage the log
                /*!
                 */
                class MongoDBDeployDataAccess:
                public data_access::DeployDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                    MongoDBUtilityDataAccess *utility_data_access = NULL;
                    MongoDBNodeDataAccess *node_data_access = NULL;
                protected:
                    MongoDBDeployDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBDeployDataAccess();
                public:
                    
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_EA932A6F_4000_47DE_9A9D_DA37C4AEC5DF_DeployDataAccess_h */

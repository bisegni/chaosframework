/*
 *	MongoDBUtilityDataAccess.h
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
#ifndef __CHAOSFramework__MongoDBUtilityDataAccess__
#define __CHAOSFramework__MongoDBUtilityDataAccess__

#include "MongoDBAccessor.h"
#include "../data_access/UtilityDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace mongodb {
                //forward declaration
                class MongoDBPersistenceDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBUtilityDataAccess:
                public data_access::UtilityDataAccess,
                protected MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                protected:
                    MongoDBUtilityDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBUtilityDataAccess();
                public:
                    //inherited method
                    virtual int getNextSequenceValue(const std::string& sequence_name, uint64_t& next_value);

                };
                
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__MongoDBUtilityDataAccess__) */

/*
 *	MongoDBObjectStorageDataAccess.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_MongoDBObjectStorageDataAccess_h
#define __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_MongoDBObjectStorageDataAccess_h

#include "ObjectStorageDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace data_service {
        namespace object_storage {
            namespace mongodb {
                
                //! Data Access for producer manipulation data
                class MongoDBObjectStorageDataAccess:
                public data_service::object_storage::abstraction::ObjectStorageDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBPersistenceDriver;
                    
                protected:
                    MongoDBObjectStorageDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBObjectStorageDataAccess();
                public:
                    //!Put an object within the object persistence layer
                    int pushObject(const std::string& key,
                                   const uint64_t& timestamp,
                                   const chaos::common::data::CDataWrapper& stored_object);
                    
                    //!Retrieve an object from the object persistence layer
                    int getObject(const std::string& key,
                                  const uint64_t& timestamp,
                                  object_storage::abstraction::ObjectSharedPtr& object_ptr_ref);
                    
                    //!search object into object persistence layer
                    int findObject(const std::string& key,
                                   const uint64_t& timestamp_from,
                                   const uint64_t& timestamp_to,
                                   const uint64_t& last_got_ts,
                                   object_storage::abstraction::VectorObject& found_object_page);
                };
            }
        }
    }
}


#endif /* __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_MongoDBObjectStorageDataAccess_h */

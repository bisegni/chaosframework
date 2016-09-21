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

#include "../abstraction/ObjectStorageDataAccess.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

namespace chaos {
    namespace data_service {
        namespace object_storage {
            namespace mongodb {
                class MongoDBObjectStorageDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBObjectStorageDataAccess:
                public data_service::object_storage::abstraction::ObjectStorageDataAccess,
                protected service_common::persistence::mongodb::MongoDBAccessor {
                    friend class MongoDBObjectStorageDriver;
                    
                    inline void addTimeRange(mongo::BSONObjBuilder& builder,
                                             const std::string& time_operator,
                                             uint64_t timestamp);
                    
                protected:
                    MongoDBObjectStorageDataAccess(const boost::shared_ptr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
                    ~MongoDBObjectStorageDataAccess();
                public:
                    //inhertied method
                    int pushObject(const std::string& key,
                                   const chaos::common::data::CDataWrapper& stored_object);
                    
                    //inhertied method
                    int getObject(const std::string& key,
                                  const uint64_t& timestamp,
                                  object_storage::abstraction::ObjectSharedPtr& object_ptr_ref);
                    //inhertied method
                    int deleteObject(const std::string& key,
                                             uint64_t start_timestamp,
                                             uint64_t end_timestamp);
                    //inhertied method
                    int findObject(const std::string& key,
                                   const uint64_t timestamp_from,
                                   const uint64_t timestamp_to,
                                   const bool from_is_included,
                                   const int page_len,
                                   object_storage::abstraction::VectorObject& found_object_page,
                                   uint64_t& last_daq_timestamp);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_MongoDBObjectStorageDataAccess_h */

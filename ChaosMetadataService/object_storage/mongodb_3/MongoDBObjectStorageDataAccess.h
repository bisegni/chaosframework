/*
 * Copyright 2012, 18/06/2018 INFN
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

#ifndef __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_MongoDBObjectStorageDataAccess_h
#define __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_MongoDBObjectStorageDataAccess_h

#include "../abstraction/ObjectStorageDataAccess.h"
#include "../object_storage_types.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <mongocxx/pool.hpp>

#include "ShardKeyManagement.h"

namespace chaos {
    namespace data_service {
        namespace object_storage {
            namespace mongodb_3 {
                class NewMongoDBObjectStorageDriver;
                
                //! Data Access for producer manipulation data
                class MongoDBObjectStorageDataAccess:
                public data_service::object_storage::abstraction::ObjectStorageDataAccess {
                    friend class NewMongoDBObjectStorageDriver;
                    mongocxx::pool& pool_ref;
                    ShardKeyManagement shrd_key_manager;

                protected:
                    MongoDBObjectStorageDataAccess(mongocxx::pool& _pool_ref);
                    ~MongoDBObjectStorageDataAccess();
                public:
                    //inhertied method
                    int pushObject(const std::string& key,
                                   const chaos::common::data::CDataWrapper& stored_object);
                    
                    //inhertied method
                    int getObject(const std::string& key,
                                  const uint64_t& timestamp,
                                  chaos::common::data::CDWShrdPtr& object_ptr_ref);
                    
                    //!inherited method
                    int getLastObject(const std::string& key,
                                      chaos::common::data::CDWShrdPtr& object_ptr_ref);
                    //inhertied method
                    int deleteObject(const std::string& key,
                                     uint64_t start_timestamp,
                                     uint64_t end_timestamp);
                    //inhertied method
                    int findObject(const std::string& key,
                                   uint64_t timestamp_from,
                                   uint64_t timestamp_to,
                                   const uint32_t page_len,
                                   object_storage::abstraction::VectorObject& found_object_page,
                                   common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq);
                    //inhertied method
                    int countObject(const std::string& key,
                                    const uint64_t timestamp_from,
                                    const uint64_t timestamp_to,
                                    const uint64_t& object_count);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_MongoDBObjectStorageDataAccess_h */

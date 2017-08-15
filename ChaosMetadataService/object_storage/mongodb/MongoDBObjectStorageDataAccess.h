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
                    MongoDBObjectStorageDataAccess(const ChaosSharedPtr<chaos::service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection);
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
                                   uint64_t timestamp_from,
                                   uint64_t timestamp_to,
                                   const uint32_t page_len,
                                   object_storage::abstraction::VectorObject& found_object_page,
                                   common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__B2D2009_02A2_468B_9C5C_1D184ECB470F_MongoDBObjectStorageDataAccess_h */

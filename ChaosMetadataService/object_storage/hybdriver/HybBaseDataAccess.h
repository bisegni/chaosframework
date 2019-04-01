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

#ifndef __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_HybBaseDataAccess_h
#define __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_HybBaseDataAccess_h

#include "../abstraction/ObjectStorageDataAccess.h"
#include "../object_storage_types.h"
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/Buffer.hpp>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <mongocxx/pool.hpp>
#include <mongocxx/bulk_write.hpp>
#include <bsoncxx/builder/basic/document.hpp>

#include "ShardKeyManagement.h"

#include <future>

namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace hybdriver {
                class NewMongoDBObjectStorageDriver;
                
                typedef struct DaqIndex {
                    std::string key;
                    CInt64 shard_value;
                    CInt64 run_id;
                    CInt64 seq_id;
                    DaqIndex& operator=(DaqIndex&& copy) {
                        key         = std::move(copy.key);
                        shard_value = std::move(copy.shard_value);
                        run_id      = std::move(copy.run_id);
                        seq_id      = std::move(copy.seq_id);
                        return *this;
                    }
                } DaqIndex;
                
                typedef struct {
                    DaqIndex index;
                    bsoncxx::builder::basic::document mongo_document;
                    chaos::common::data::BufferUPtr data_blob;
                } DaqBlob;
                
                typedef ChaosSharedPtr<DaqBlob> DaqBlobSPtr;
                
                //! Data Access for producer manipulation data
                class HybBaseDataAccess:
                public metadata_service::object_storage::abstraction::ObjectStorageDataAccess {
                    friend class HybBaseDriver;
                    //size of the batch
                    CUInt32 batch_size;
                    //batch timeout in milliseconds
                    CUInt32 batch_timeout;
                    CInt64  next_timeout;
                    mongocxx::bulk_write _bulk_write;
                    ChaosSharedPtr<mongocxx::pool> pool_ref;
                    ShardKeyManagement shrd_key_manager;
                    
                    ChaosUniquePtr<std::set<DaqBlobSPtr>> blob_set_uptr;
                    std::future<void> current_push_future;
                    
                    virtual int storeData(const DaqBlob& daq_blob) = 0;
                    
                    virtual int retrieveData(const DaqIndex& index,
                                             chaos::common::data::CDWUniquePtr& object) = 0;
                    //execute the push of the data
                    void executePush(ChaosUniquePtr<std::set<DaqBlobSPtr>> _blob_set_uptr);
                public:
                    HybBaseDataAccess();
                    ~HybBaseDataAccess();
                    //inhertied method
                    int pushObject(const std::string&                       key,
                                   const ChaosStringSetConstSPtr            meta_tags,
                                   const chaos::common::data::CDataWrapper& stored_object);
                    
                    //inhertied method
                    int getObject(const std::string&               key,
                                  const uint64_t&                  timestamp,
                                  chaos::common::data::CDWShrdPtr& object_ptr_ref);
                    
                    //!inherited method
                    int getLastObject(const std::string&               key,
                                      chaos::common::data::CDWShrdPtr& object_ptr_ref);
                    //inhertied method
                    int deleteObject(const std::string& key,
                                     uint64_t           start_timestamp,
                                     uint64_t           end_timestamp);
                    //inhertied method
                    int findObject(const std::string&                                              key,
                                   const ChaosStringSet&                                           meta_tags,
                                   const uint64_t                                                  timestamp_from,
                                   const uint64_t                                                  timestamp_to,
                                   const uint32_t                                                  page_len,
                                   metadata_service::object_storage::abstraction::VectorObject&    found_object_page,
                                   common::direct_io::channel::opcode_headers::SearchSequence&     last_record_found_seq);
                    
                    //inhertied method
                    int findObjectIndex(const abstraction::DataSearch& search,
                                        abstraction::VectorObject& found_object_page,
                                        chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq);
                    
                    //inhertied method
                    int getObjectByIndex(const abstraction::VectorObject& search,
                                         abstraction::VectorObject& found_object_page);
                    
                    //inhertied method
                    int countObject(const std::string& key,
                                    const uint64_t     timestamp_from,
                                    const uint64_t     timestamp_to,
                                    const uint64_t&    object_count);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_HybBaseDataAccess_h */

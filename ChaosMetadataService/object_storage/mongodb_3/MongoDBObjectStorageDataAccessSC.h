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

#ifndef __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_MongoDBObjectStorageDataAccessSC_h
#define __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_MongoDBObjectStorageDataAccessSC_h

#include "../abstraction/ObjectStorageDataAccess.h"
#include "../object_storage_types.h"

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>

#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <mongocxx/pool.hpp>
#include "ShardKeyManagement.h"

#if CHAOS_PROMETHEUS
#include <chaos/common/metric/metric.h>
#endif //CHAOS_PROMETHEUS
namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace mongodb_3 {
                class NewMongoDBObjectStorageDriver;
                
                typedef ChaosSharedPtr<bsoncxx::builder::basic::document> BlobShrdPtr;
                
                CHAOS_DEFINE_LOCKABLE_OBJECT(std::set<BlobShrdPtr>, BlobSetL);
                //! define a lockable seet for betch entries
                
                
#if CHAOS_PROMETHEUS
                template<typename ReturnType>
                ReturnType computeTimeForOperationInGauge(chaos::common::metric::GaugeUniquePtr& gauge,
                                                          std::function<ReturnType()> function) {
                    boost::posix_time::ptime start = boost::posix_time::second_clock::local_time();
                    ReturnType ret = function();
                    boost::posix_time::ptime end = boost::posix_time::second_clock::local_time();
                    (*gauge) = (end - start).total_milliseconds();
                    return ret;
                }
#endif
                
                //! Data Access for producer manipulation data
                class MongoDBObjectStorageDataAccessSC:
                public metadata_service::object_storage::abstraction::ObjectStorageDataAccess,
                public chaos::common::async_central::TimerHandler {
                    friend class NewMongoDBObjectStorageDriver;
                    mongocxx::pool&     pool_ref;
                    ShardKeyManagement  shrd_key_manager;
                    
                    //!push data using batch operation
                    uint64_t            curret_batch_size;
                    uint64_t            batch_size_limit;
                    int32_t             push_timeout_multiplier;
                    int32_t             push_current_step_left;
                    BlobSetL            batch_set;
                    uint32_t            write_timeout,read_timeout;
                    std::string         search_hint_name;
                    
                    mongocxx::write_concern       write_options;
                    std::future<void> current_push_future;
                    
                    //metric
#if CHAOS_PROMETHEUS
                    chaos::common::metric::CounterUniquePtr counter_write_data_uptr;
                    chaos::common::metric::CounterUniquePtr counter_read_data_uptr;
                    chaos::common::metric::GaugeUniquePtr gauge_insert_time_uptr;
                    chaos::common::metric::GaugeUniquePtr gauge_query_time_uptr;
#endif
                protected:
                    MongoDBObjectStorageDataAccessSC(mongocxx::pool& _pool_ref);
                    ~MongoDBObjectStorageDataAccessSC();
                    
                    
                    void executePush(std::set<BlobShrdPtr>&& _batch_element_to_store);
                    //!TimeOutHnadler inherited
                    void timeout();
                    
                    inline chaos::common::data::CDWShrdPtr getDataByID(mongocxx::database& db,
                                                                       const std::string& _id);
                    
                    inline abstraction::VectorObject getDataByID(mongocxx::database& db,
                                                                 const ChaosStringSet& _ids);
                public:
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
                    int getObjectByIndex(const chaos::common::data::CDWShrdPtr& index,
                                         chaos::common::data::CDWShrdPtr& found_object);
                    
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

#endif /* __CHAOSFramework_E927A5B7_1CA0_802F_AA20_DD63646EA30A_MongoDBObjectStorageDataAccessSC_h */

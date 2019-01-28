/*
 * Copyright 2012, 30/11/2018 INFN
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

#ifndef __CHAOSFramework_E92255B7_1CA0_AAAA_AA20_DD63646EA30A_CassandraObjectStorageDataAccess_h
#define __CHAOSFramework_E92255B7_1CA0_AAAA_AA20_DD63646EA30A_CassandraObjectStorageDataAccess_h
#if USE_CASSANDRA_DRIVER
#include "CassandraObjectStorageTypes.h"
#include "../abstraction/ObjectStorageDataAccess.h"
#include "../object_storage_types.h"
#include <chaos/common/utility/ObjectInstancer.h>
#include <chaos_service_common/persistence/mongodb/MongoDBAccessor.h>

#include <cassandra.h>
namespace chaos {
    namespace metadata_service {
        namespace object_storage {
            namespace cassandra {
                class CassandraObjectStorageDriver;
                
                //! Data Access for producer manipulation data
                class CassandraObjectStorageDataAccess:
                public metadata_service::object_storage::abstraction::ObjectStorageDataAccess {
                    friend class CassandraObjectStorageDriver;
                    CassSessionShrdPtr cass_sess_shrd_ptr;

                    const CassPrepared *insert_daq_prepared;
                    const CassPrepared *get_daq_prepared;
                    const CassPrepared *delete_daq_prepared;
                    const CassPrepared *get_last_daq_prepared;
                    const CassPrepared *find_daq_prepared;
                    const CassPrepared *find_daq_prepared_reverse;
                    const CassPrepared *count_daq_prepared;



//                    ShardKeyManagement shrd_key_manager;
                    
                protected:
                    CassandraObjectStorageDataAccess(CassSessionShrdPtr& _cass_sess_shrd_ptr);
                    ~CassandraObjectStorageDataAccess();
                public:
                    //inhertied method
                    /*!
                        partition key device_id
                        cluster run_id, seq_id, ats
                     */
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
                    int countObject(const std::string& key,
                                    const uint64_t     timestamp_from,
                                    const uint64_t     timestamp_to,
                                    const uint64_t&    object_count);
                };
            }
        }
    }
}
#endif
#endif /* __CHAOSFramework_E92255B7_1CA0_AAAA_AA20_DD63646EA30A_CassandraObjectStorageDataAccess_h */

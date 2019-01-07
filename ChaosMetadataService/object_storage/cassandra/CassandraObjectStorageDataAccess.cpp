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
#if USE_CASSANDRA_DRIVER
#include "CassandraObjectStorageDataAccess.h"

#include <chaos/common/global.h>

#define INFO INFO_LOG(CassandraObjectStorageDataAccess)
#define DBG  DBG_LOG(CassandraObjectStorageDataAccess)
#define ERR  ERR_LOG(CassandraObjectStorageDataAccess)

#define DEFAULT_QUANTIZATION        100

using namespace chaos::metadata_service::object_storage::cassandra;
using namespace chaos::metadata_service::object_storage::abstraction;

CassandraObjectStorageDataAccess::CassandraObjectStorageDataAccess(CassSessionShrdPtr& _cass_sess_shrd_ptr):
cass_sess_shrd_ptr(_cass_sess_shrd_ptr){
    CassError err = CASS_OK;
    /* Prepare the statement on the Cassandra cluster */
    CassFuture* prepare_future = cass_session_prepare(cass_sess_shrd_ptr.get(), "INSERT INTO daq (did, pid, ats, rid, sid, data) VALUES (?, ?, ?, ?, ?, ?);");
    if ((err = cass_future_error_code(prepare_future)) == CASS_OK) {
        insert_daq_prepared = cass_future_get_prepared(prepare_future);
    }
    cass_future_free(prepare_future);
    if(err != CASS_OK) {
        LOG_AND_TROW(ERR, -2, cass_error_desc(err))
    }
}

CassandraObjectStorageDataAccess::~CassandraObjectStorageDataAccess() {
    cass_prepared_free(insert_daq_prepared);
}

int CassandraObjectStorageDataAccess::pushObject(const std::string&                       key,
                                                 const ChaosStringSetConstSPtr            meta_tags,
                                                 const chaos::common::data::CDataWrapper& stored_object) {
    
    /* The prepared object can now be used to create statements that can be executed */
    CassStatement* statement = cass_prepared_bind(insert_daq_prepared);
    
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_string_by_name(statement, "key", "abc");
    cass_statement_bind_int32_by_name(statement, "value", 123);
    
    /* Execute statement (same a the non-prepared code) */

    return 0;
}

int CassandraObjectStorageDataAccess::getObject(const std::string&               key,
                                                const uint64_t&                  timestamp,
                                                chaos::common::data::CDWShrdPtr& object_ptr_ref) {
    return 0;
}

int CassandraObjectStorageDataAccess::getLastObject(const std::string&               key,
                                                    chaos::common::data::CDWShrdPtr& object_ptr_ref) {
    return 0;
}

int CassandraObjectStorageDataAccess::deleteObject(const std::string& key,
                                                   uint64_t           start_timestamp,
                                                   uint64_t           end_timestamp) {
    return 0;
}

int CassandraObjectStorageDataAccess::findObject(const std::string&                                              key,
                                                 const ChaosStringSet&                                           meta_tags,
                                                 const uint64_t                                                  timestamp_from,
                                                 const uint64_t                                                  timestamp_to,
                                                 const uint32_t                                                  page_len,
                                                 metadata_service::object_storage::abstraction::VectorObject&    found_object_page,
                                                 common::direct_io::channel::opcode_headers::SearchSequence&     last_record_found_seq) {
    return 0;
}

int CassandraObjectStorageDataAccess::countObject(const std::string& key,
                                                  const uint64_t     timestamp_from,
                                                  const uint64_t     timestamp_to,
                                                  const uint64_t&    object_count) {
    return 0;
}
#endif

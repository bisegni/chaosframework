/*
 * Copyright 2012, 25/05/2018 INFN
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

#include "CassHybDataAccess.h"

#define DAQ_SHARD_FIELD "shard"
#define DAQ_DATA_FIELD  "data"
#define DAQ_TABLE_NAME  "daq"

#define INFO    INFO_LOG(CassHybDataAccess)
#define DBG     DBG_LOG(CassHybDataAccess)
#define ERR     ERR_LOG(CassHybDataAccess)

#define CHECK_FUTURE(x, pf, err1) \
if((err = cass_future_error_code(pf.get())) != CASS_OK) {LOG_AND_TROW(ERR, err1, cass_error_desc(err));}\
x = MAKE_MANAGED_PREPARED(const_cast<CassPrepared*>(cass_future_get_prepared(pf.get())));

using namespace chaos::common::data;

using namespace chaos::metadata_service::object_storage::hybdriver;
using namespace chaos::metadata_service::object_storage::hybdriver::cassandra;

CassHybDataAccess::CassHybDataAccess(CassSessionShrdPtr& _cass_sess_shrd_ptr):
session_shrd_ptr(_cass_sess_shrd_ptr){
    CassError err = CASS_OK;
    MAKE_MANAGED_FUTURE(prepare_future, cass_session_prepare(session_shrd_ptr.get(), CHAOS_FORMAT("INSERT INTO %1% (%2%, %3%, %4%, %5%, %6%) VALUES (?, ?, ?, ?, ?)",
                                                                                                  %DAQ_TABLE_NAME
                                                                                                  %chaos::DataPackCommonKey::DPCK_DEVICE_ID
                                                                                                  %DAQ_SHARD_FIELD
                                                                                                  %chaos::ControlUnitDatapackCommonKey::RUN_ID
                                                                                                  %chaos::DataPackCommonKey::DPCK_SEQ_ID
                                                                                                  %DAQ_DATA_FIELD).c_str()));
    CHECK_FUTURE(insert_daq_prepared, prepare_future, -1);
    /* GET: */
    MAKE_MANAGED_FUTURE(prepare_future_1, cass_session_prepare(session_shrd_ptr.get(), CHAOS_FORMAT("SELECT %1% FROM %2% WHERE %3% = ? and %4% = ? and %5% = ? and %6% = ?",
                                                                                                    %DAQ_DATA_FIELD
                                                                                                    %DAQ_TABLE_NAME
                                                                                                    %chaos::DataPackCommonKey::DPCK_DEVICE_ID
                                                                                                    %DAQ_SHARD_FIELD
                                                                                                    %chaos::ControlUnitDatapackCommonKey::RUN_ID
                                                                                                    %chaos::DataPackCommonKey::DPCK_SEQ_ID).c_str()));
    CHECK_FUTURE(get_daq_prepared, prepare_future_1, -2);
    
    //delete
    MAKE_MANAGED_FUTURE(prepare_future_2, cass_session_prepare(session_shrd_ptr.get(), CHAOS_FORMAT("DELETE FROM %1% WHERE %2% = ? and %3% = ? and %4% = ? and %5% = ?",
                                                                                                    %DAQ_TABLE_NAME
                                                                                                    %chaos::DataPackCommonKey::DPCK_DEVICE_ID
                                                                                                    %DAQ_SHARD_FIELD
                                                                                                    %chaos::ControlUnitDatapackCommonKey::RUN_ID
                                                                                                    %chaos::DataPackCommonKey::DPCK_SEQ_ID).c_str()));
    CHECK_FUTURE(delete_daq_prepared, prepare_future_2, -3);
}

int CassHybDataAccess::storeData(const std::set<DaqBlobSPtr>& blob_set) {
    CassError err = CASS_OK;
    //    CassBatch* batch = cass_batch_new(CASS_BATCH_TYPE_LOGGED);
    std::set<CassFutureShrdPtr> futures;
    std::for_each(blob_set.begin(), blob_set.end(), [=, &futures](const DaqBlobSPtr& blob){
        /* The prepared object can now be used to create statements that can be executed */
        CassStatementShrdPtr statement = MAKE_MANAGED_STATEMENT(cass_prepared_bind(this->insert_daq_prepared.get()));
        
        /* Bind variables by name this time (this can only be done with prepared statements)*/
        cass_statement_bind_string_by_name(statement.get(),
                                           chaos::DataPackCommonKey::DPCK_DEVICE_ID,
                                           blob->index.key.c_str());
        cass_statement_bind_int64_by_name(statement.get(),
                                          DAQ_SHARD_FIELD,
                                          blob->index.shard_value);
        cass_statement_bind_int64_by_name(statement.get(),
                                          chaos::ControlUnitDatapackCommonKey::RUN_ID,
                                          blob->index.run_id);
        cass_statement_bind_int64_by_name(statement.get(),
                                          chaos::DataPackCommonKey::DPCK_SEQ_ID,
                                          blob->index.seq_id);
        
        cass_statement_bind_bytes_by_name(statement.get(),
                                          DAQ_DATA_FIELD,
                                          (const std::uint8_t*)blob->data_blob->data(),
                                          blob->data_blob->size());
        
        MAKE_MANAGED_FUTURE(query_future, cass_session_execute(session_shrd_ptr.get(), statement.get()));
        futures.insert(query_future);
    });
    
    std::for_each(futures.begin(), futures.end(), [&err](const CassFutureShrdPtr& future){
        if((err = cass_future_error_code(future.get()))) {
            printf("Query result: %s\n", cass_error_desc(err));
        }
    });
    //wait on futures
    return err;
}

int CassHybDataAccess::retrieveData(const DaqIndex& index,
                                    CDWUniquePtr& object) {
    /* The prepared object can now be used to create statements that can be executed */
    CassStatementShrdPtr statement = MAKE_MANAGED_STATEMENT(cass_prepared_bind(this->get_daq_prepared.get()));
    
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_string_by_name(statement.get(),
                                       chaos::DataPackCommonKey::DPCK_DEVICE_ID,
                                       index.key.c_str());
    cass_statement_bind_int64_by_name(statement.get(),
                                      DAQ_SHARD_FIELD,
                                      index.shard_value);
    cass_statement_bind_int64_by_name(statement.get(),
                                      chaos::ControlUnitDatapackCommonKey::RUN_ID,
                                      index.run_id);
    cass_statement_bind_int64_by_name(statement.get(),
                                      chaos::DataPackCommonKey::DPCK_SEQ_ID,
                                      index.seq_id);
    
    MAKE_MANAGED_FUTURE(query_future, cass_session_execute(session_shrd_ptr.get(), statement.get()));
    
    const CassResult* result = cass_future_get_result(query_future.get());
    const CassRow* row = cass_result_first_row(result);
    const CassValue* daq_column = cass_row_get_column(row, 0);
    size_t buffer_size;
    const cass_byte_t *buffer;
    cass_value_get_bytes(daq_column, &buffer, &buffer_size);
    object = CDWUniquePtr(new CDataWrapper((const char *)buffer));
    cass_result_free(result);
    return 0;
}

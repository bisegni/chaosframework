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
#define DAQ_DATA_FIELD      "data"
#define DAQ_COLL_NAME       "daq"

using namespace chaos::metadata_service::object_storage::cassandra;
using namespace chaos::metadata_service::object_storage::abstraction;


#define CHECK_FUTURE(x, pf, err1, err2) \
if((err = cass_future_error_code(pf.get())) != CASS_OK) {LOG_AND_TROW(ERR, err1, cass_error_desc(err));}\
x = MAKE_MANAGED_PREPARED(cass_future_get_prepared(pf.get()));


/* Constructor */
CassandraObjectStorageDataAccess::CassandraObjectStorageDataAccess(CassSessionShrdPtr& _cass_sess_shrd_ptr):
cass_sess_shrd_ptr(_cass_sess_shrd_ptr){
    CassError err = CASS_OK;


    /* INSERT: Prepare the statement on the Cassandra cluster */
    MAKE_MANAGED_FUTURE(prepare_future, cass_session_prepare(cass_sess_shrd_ptr.get(), CHAOS_FORMAT("INSERT INTO %1% (%2%, %3%, %4%, %5%, %6%) VALUES (?, ?, ?, ?, ?)",
                                                                                                        %DAQ_COLL_NAME
                                                                                                        %chaos::DataPackCommonKey::DPCK_DEVICE_ID
                                                                                                        %chaos::DataPackCommonKey::DPCK_TIMESTAMP
                                                                                                        %chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID
                                                                                                        %chaos::DataPackCommonKey::DPCK_SEQ_ID
                                                                                                        %DAQ_DATA_FIELD).c_str()));
    
    CHECK_FUTURE(insert_daq_prepared, prepare_future, -1, -2);
    
    /* GET: */
    MAKE_MANAGED_FUTURE(prepare_future_1, cass_session_prepare(cass_sess_shrd_ptr.get(), CHAOS_FORMAT("SELECT * FROM %1% WHERE %2% = ? and %3% = ?",
                                                                                                     %DAQ_COLL_NAME
                                                                                                     %chaos::DataPackCommonKey::DPCK_DEVICE_ID
                                                                                                     %chaos::DataPackCommonKey::DPCK_TIMESTAMP).c_str()));
    CHECK_FUTURE(get_daq_prepared, prepare_future_1, -2, -3);
    
    
    /* DELETE: */
    MAKE_MANAGED_FUTURE(prepare_future_2, cass_session_prepare(cass_sess_shrd_ptr.get(), " DELETE  FROM "+DAQ_COLL_NAME+
                                                                                         " WHERE "+std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" >= ? 
                                                                                           and "+td::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" <= ? IF EXISTS "));
    CHECK_FUTURE(delete_daq_prepared, prepare_future_2, -2, -3);


    /* GET LAST: */
    MAKE_MANAGED_FUTURE(prepare_future_3,cass_session_prepare(cass_sess_shrd_ptr.get(), " SELECT * FROM "+DAQ_COLL_NAME+
                                                                                        " WHERE "+std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID)+
                                                                                        " = ? AND "+td::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" = ? LIMIT 1 "));
    CHECK_FUTURE(get_last_daq_prepared, prepare_future_3, -2, -3);


    /* FIND the first page_len records between timestamp_from and timestamp_to */
    const std::string run_key     = CHAOS_FORMAT("%1%.%2%",%DAQ_DATA_FIELD%chaos::ControlUnitDatapackCommonKey::RUN_ID);
    const std::string counter_key = CHAOS_FORMAT("%1%.%2%",%DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID);
    //NORMAL
    MAKE_MANAGED_FUTURE(prepare_future_4, cass_session_prepare(cass_sess_shrd_ptr.get(), " SELECT * FROM "+DAQ_COLL_NAME+
                                                                                         " WHERE "+std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" >= ? AND "+td::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" <= ? and "+run_key+" >= ? and "+counter_key+ " >= ?"));
    CHECK_FUTURE(find_daq_prepared, prepare_future_4, -2, -3);

    //REVERSE
    MAKE_MANAGED_FUTURE(prepare_future_5, cass_session_prepare(cass_sess_shrd_ptr.get(), " SELECT * FROM "+DAQ_COLL_NAME+
                                                                                         " WHERE "+std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" >= ? AND "+td::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP)+" <= ? "+" and "+run_key+" <= ? and "+counter_key+ " <= ?"));
    CHECK_FUTURE(find_daq_prepared_reverse, prepare_future_5, -2, -3);
    /* COUNT: */
    MAKE_MANAGED_FUTURE(prepare_future_6, cass_session_prepare(cass_sess_shrd_ptr.get(), "SELECT COUNT(*) FROM "+DAQ_COLL_NAME));
    CHECK_FUTURE(count_daq_prepared, prepare_future_6, -2, -3);


}

CassandraObjectStorageDataAccess::~CassandraObjectStorageDataAccess() {}

int CassandraObjectStorageDataAccess::pushObject(const std::string&                       key,
                                                 const ChaosStringSetConstSPtr            meta_tags,
                                                 const chaos::common::data::CDataWrapper& stored_object) {
    
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;

    /* The prepared object can now be used to create statements that can be executed */
    CassStatement* statement = cass_prepared_bind(insert_daq_prepared);
    
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_string_by_name(statement, 
                                       std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key);
    cass_statement_bind_int64_by_name(statement,  
                                      chaos::DataPackCommonKey::DPCK_TIMESTAMP, now_in_ms);
    cass_statement_bind_int64_by_name(statement, 
                                       std::string(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID), 
                                       stored_object.getIntValue(chaos::ControlUnitNodeDefinitionKey::CONTROL_UNIT_RUN_ID));
    cass_statement_bind_int64_by_name(statement, 
                                      std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), 
                                      stored_object.getIntValue(chaos::DataPackCommonKey::DPCK_SEQ_ID));


    /*  CassError cass_statement_bind_bytes_by_name ( CassStatement * statement, const char * name, const cass_byte_t * value, size_t value_size )  */

    cass_statement_bind_bytes_by_name(statement, 
                                      std::string(DAQ_DATA_FIELD), 
                                      (const std::uint8_t*)stored_object.getBSONRawData(), 
                                      stored_object.getBSONRawSize());

    CassFuture* query_future = cass_session_execute(cass_sess_shrd_ptr, statement);
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);
    /* log the query */
    printf("Query result: %s\n", cass_error_desc(rc));
    
    cass_future_free(query_future);

    return 0;
}

int CassandraObjectStorageDataAccess::getObject(const std::string&               key,
                                                const uint64_t&                  timestamp,
                                                chaos::common::data::CDWShrdPtr& object_ptr_ref) {


    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;

    /* The prepared object can now be used to create statements that can be executed */
    CassStatement* statement = cass_prepared_bind(get_daq_prepared);
    
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_string_by_name(statement, std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key);
    cass_statement_bind_int64_by_name(statement,  chaos::DataPackCommonKey::DPCK_TIMESTAMP, now_in_ms);
    CassFuture* query_future = cass_session_execute(cass_sess_shrd_ptr, statement);
        
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    
    /* This will also block until the query returns */
    const CassResult* result = cass_future_get_result(query_future);
    
    /* If there was an error then the result won't be available */
    if (result == NULL) {
        /* Handle error */
        cass_future_free(query_future);
        return -1;
    }


    /* The future can be freed immediately after getting the result object */
    cass_future_free(query_future);
    
    /* This can be used to retrieve on the first row of the result */
    const CassRow* row = cass_result_first_row(result);
    
    /*GET the result*/
    /* Now we can retrieve the column values from the row */
    CassString key;
    /* Get the column value of "key" by name */
    cass_value_get_string(cass_row_get_column_by_name(row, std::string(DAQ_DATA_FIELD)), &key);
    cass_byte_t value;
    /* Get the column value of "value" by name */
    //TODO
    cass_value_get_bytes(cass_row_get_column_by_name(row, "value"), &value);
    object_ptr_ref.reset(new CDataWrapper(value.data()));

    
    /* This will free the future as well as the string pointed to by the CassString 'key' */
    cass_result_free(result);
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);
    cass_future_free(query_future);
    
    return 0;
}

int CassandraObjectStorageDataAccess::getLastObject(const std::string&               key,
                                                    chaos::common::data::CDWShrdPtr& object_ptr_ref) {
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;

    /* The prepared object can now be used to create statements that can be executed */
    CassStatement* statement = cass_prepared_bind(get_daq_prepared);
    
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_string_by_name(statement, std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key);
    cass_statement_bind_int64_by_name(statement,  chaos::DataPackCommonKey::DPCK_TIMESTAMP, now_in_ms);
    CassFuture* query_future = cass_session_execute(cass_sess_shrd_ptr, statement);
        
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    
    /* This will also block until the query returns */
    const CassResult* result = cass_future_get_result(query_future);
    
    /* If there was an error then the result won't be available */
    if (result == NULL) {
        /* Handle error */
        cass_future_free(query_future);
        return -1;
    }


    /* The future can be freed immediately after getting the result object */
    cass_future_free(query_future);
    
    /* This can be used to retrieve on the first row of the result */
    const CassRow* row = cass_result_first_row(result);
    
    /*GET the result*/
    /* Now we can retrieve the column values from the row */
    CassString key;
    /* Get the column value of "key" by name */
    cass_value_get_string(cass_row_get_column_by_name(row, std::string(DAQ_DATA_FIELD)), &key);
    cass_byte_t value;
    /* Get the column value of "value" by name */
    //TODO
    cass_value_get_bytes(cass_row_get_column_by_name(row, "value"), &value);
    object_ptr_ref.reset(new CDataWrapper(value.data()));

    
    /* This will free the future as well as the string pointed to by the CassString 'key' */
    cass_result_free(result);
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);
    cass_future_free(query_future);
    
    return 0;
    
    
}


int CassandraObjectStorageDataAccess::deleteObject(const std::string& key,
                                                   uint64_t           start_timestamp,
                                                   uint64_t           end_timestamp) {
    
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;

    /* The prepared object can now be used to create statements that can be executed */
    CassStatement* statement = cass_prepared_bind(delete_daq_prepared);
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_int64_by_name(statement,  
                                      chaos::DataPackCommonKey::DPCK_TIMESTAMP, start_timestamp);
    cass_statement_bind_int64_by_name(statement,  
                                      chaos::DataPackCommonKey::DPCK_TIMESTAMP, end_timestamp);

    CassFuture* query_future = cass_session_execute(cass_sess_shrd_ptr, statement);
        
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);
    /* log the query */
    printf("Query result: %s\n", cass_error_desc(rc));
    cass_future_free(query_future);

    return 0;

}

auto get_now

int CassandraObjectStorageDataAccess::findObject(const std::string&                                              key,
                                                 const ChaosStringSet&                                           meta_tags,
                                                 const uint64_t                                                  timestamp_from,
                                                 const uint64_t                                                  timestamp_to,
                                                 const uint32_t                                                  page_len,
                                                 metadata_service::object_storage::abstraction::VectorObject&    found_object_page,
                                                 common::direct_io::channel::opcode_headers::SearchSequence&     last_record_found_seq) {
    
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;

    /* The prepared object can now be used to create statements that can be executed */
    const std::string run_key = CHAOS_FORMAT("%1%.%2%",%DAQ_DATA_FIELD%chaos::ControlUnitDatapackCommonKey::RUN_ID);
    const std::string counter_key = CHAOS_FORMAT("%1%.%2%",%DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID)

    bool reverse_order = false;
    //we have the intervall
    reverse_order = timestamp_from>timestamp_to;
    if(reverse_order){
        CassStatement* statement = cass_prepared_bind(find_daq_prepared);
    }else{
        CassStatement* statement = cass_prepared_bind(find_daq_prepared_reverse);
    }
        
    /* Return a page_len rows every time this statement is executed */
    cass_statement_set_paging_size(statement, page_len);
    
    
    /* Bind variables by name this time (this can only be done with prepared statements)*/
    cass_statement_bind_int64_by_name(statement, 
                                       std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), timestamp_from);
    cass_statement_bind_int64_by_name(statement,  
                                      chaos::DataPackCommonKey::DPCK_TIMESTAMP, timestamp_to);

    cass_statement_bind_int64_by_name(statement, run_key, last_record_found_seq.run_id));
    cass_statement_bind_int64_by_name(statement, counter_key, last_record_found_seq.datapack_counter );                                  

    //Add metatag



    CassFuture* query_future = cass_session_execute(cass_sess_shrd_ptr, statement);
        
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    /* This will also block until the query returns */
    const CassResult* result = cass_future_get_result(query_future);
    
    /* If there was an error then the result won't be available */
    if (result == NULL) {
        /* Handle error */
        cass_future_free(query_future);
        return -1;
    }


    /* The future can be freed immediately after getting the result object */
    cass_future_free(query_future);
    
    /* This can be used to retrieve on the first row of the result */
    const CassRow* row = cass_result_first_row(result);
    
    /*GET the result*/
    /* Now we can retrieve the column values from the row */
    CassString key;
    /* Get the column value of "key" by name */
    cass_value_get_string(cass_row_get_column_by_name(row, std::string(DAQ_DATA_FIELD)), &key);
    cass_byte_t value;
    /* Get the column value of "value" by name */
    //TODO
    cass_value_get_bytes(cass_row_get_column_by_name(row, "value"), &value);
    object_ptr_ref.reset(new CDataWrapper(value.data()));

    
    /* This will free the future as well as the string pointed to by the CassString 'key' */
    cass_result_free(result);
    /* Statement objects can be freed immediately after being executed */
    cass_statement_free(statement);
    /* This will block until the query has finished */
    CassError rc = cass_future_error_code(query_future);
    cass_future_free(query_future);
    
    return 0;
}

int CassandraObjectStorageDataAccess::countObject(const std::string& key,
                                                  const uint64_t     timestamp_from,
                                                  const uint64_t     timestamp_to,
                                                  const uint64_t&    object_count) {
    return 0;
}
#endif

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
#include <chaos/common/global.h>

#include "MongoDBObjectStorageDataAccess.h"
#include "../../ChaosMetadataService.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>


#include <iostream>

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/options/find.hpp>
#include <bsoncxx/document/view.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/builder/basic/array.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/server_error_code.hpp>
#include <mongocxx/exception/logic_error.hpp>

//using namespace chaos::data_service::object_storage::mongodb;

#define INFO INFO_LOG(MongoDBObjectStorageDataAccess)
#define DBG  DBG_LOG(MongoDBObjectStorageDataAccess)
#define ERR  ERR_LOG(MongoDBObjectStorageDataAccess)

static constexpr char MONGODB_DB_NAME[]                 = "chaos";
static constexpr char MONGODB_DAQ_COLL_NAME[]           = "daq";
static constexpr char MONGODB_DAQ_INDEX_COLL_NAME[]     = "daq_index";
static constexpr char MONGODB_DAQ_DATA_FIELD[]          = "data";
static constexpr int  DEFAULT_BATCH_SIZE_IN_BYTE        = 1*1024*1024; // 1 mbyte
static constexpr int  DEFAULT_BATCH_TIMEOUT_MULTIPLIER  = 0;           // 1 seconds

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_date;
using bsoncxx::builder::stream::close_array;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::open_document;

using namespace mongocxx;
using namespace bsoncxx;
using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;
using namespace chaos::common::batch_command;
using namespace chaos::common::direct_io::channel::opcode_headers;

//using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::object_storage::mongodb_3;
using namespace chaos::metadata_service::object_storage::abstraction;


static bool findIndex(mongocxx::database& db,
                      const std::string& col_name,
                      const std::string& name) {
    bool found = false;
    auto index_cur = db[col_name].list_indexes();
    for(auto && document : index_cur){
        auto index_name = document["name"];
        if(index_name.type() != bsoncxx::type::k_utf8){
            continue;
        }
        if((found = index_name.get_utf8().value.to_string().compare(name) == 0)){
            break;
        }
    }
    return found;
}

static void initShardIndex(mongocxx::database& db,
                           const std::string& col_name) {
    if(findIndex(db,
                 col_name,
                 "shard_index")) {
        return;
    }
    auto index_builder = builder::basic::document{};
    mongocxx::options::index index_options{};
    index_builder.append(kvp("zone_key", 1));
    index_builder.append(kvp("shard_key", 1));
    index_options.name("shard_index");
    try{
        db[col_name].create_index(index_builder.view(), index_options);
    } catch( mongocxx::operation_exception& e) {
        //fail to create index
        ERR << e.what();
    }
}
static void initSearchIndex(mongocxx::database& db,
                            std::string col_name) {
    if(findIndex(db,
                 col_name,
                 "paged_daq_seq_search_index")) {
        return;
    }
    auto index_builder = builder::basic::document{};
    mongocxx::options::index index_options{};
    index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), 1));
    index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), 1));
    index_builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), 1));
    index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), 1));
    index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), 1));
    index_options.name("paged_daq_seq_search_index");
    try{
        db[col_name].create_index(index_builder.view(), index_options);
    } catch( mongocxx::operation_exception& e) {
        //fail to create index
        ERR << e.what();
    }
}

static void initSearchData(mongocxx::database& db,
                           std::string col_name) {
    if(findIndex(db,
                 col_name,
                 "order_index")) {
        return;
    }
    auto index_builder = builder::basic::document{};
    mongocxx::options::index index_options{};
    index_builder.append(kvp(CHAOS_FORMAT("data.%1%",%chaos::ControlUnitDatapackCommonKey::RUN_ID), 1));
    index_builder.append(kvp(CHAOS_FORMAT("data.%1%",%chaos::DataPackCommonKey::DPCK_SEQ_ID), 1));
    index_options.name("order_index");
    try{
        db[col_name].create_index(index_builder.view(), index_options);
    } catch( mongocxx::operation_exception& e) {
        //fail to create index
        ERR << e.what();
    }
}


MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(pool& _pool_ref):
pool_ref(_pool_ref),
curret_batch_size(0),
batch_size_limit(DEFAULT_BATCH_SIZE_IN_BYTE),
push_timeout_multiplier(DEFAULT_BATCH_TIMEOUT_MULTIPLIER),
push_current_step_left(push_timeout_multiplier),write_timeout(common::constants::ObjectStorageTimeoutinMSec),read_timeout(common::constants::ObjectStorageTimeoutinMSec){
    //get client the connection
    auto client = pool_ref.acquire();
    
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    
    //index collection
    initShardIndex(db, MONGODB_DAQ_INDEX_COLL_NAME);
    initSearchIndex(db, MONGODB_DAQ_INDEX_COLL_NAME);
    
    //data collection
    initShardIndex(db, MONGODB_DAQ_COLL_NAME);
    initSearchData(db, MONGODB_DAQ_COLL_NAME);
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    if(obj_stoarge_kvp.count("wtimeout")) {
        write_timeout=strtoul(obj_stoarge_kvp["wtimeout"].c_str(),0,0);
        
        write_options.majority(std::chrono::milliseconds(write_timeout));
        DBG<<" setting write timeout:"<<write_timeout<<" ms";
    }
    if(obj_stoarge_kvp.count("rtimeout")) {
        read_timeout=strtoul(obj_stoarge_kvp["wtimeout"].c_str(),0,0);
        DBG<<" setting read timeout:"<<read_timeout<<" ms";
    }
    if(obj_stoarge_kvp.count("mongodb_oswc")) {
        //set the custom write concern
        INFO << CHAOS_FORMAT("Set MongoDB object storage write concern to %1%", %obj_stoarge_kvp["mongodb_oswc"]);
        if(obj_stoarge_kvp["mongodb_oswc"].compare("unacknowledged") == 0) {
            write_options.acknowledge_level(write_concern::level::k_unacknowledged);
            DBG<<" Write Unknowledged";
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("acknowledged") == 0) {
            DBG<<" Write Aknowledged";
            write_options.acknowledge_level(write_concern::level::k_acknowledged);
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("journaled") == 0) {
            DBG<<" Write Journaled";
            write_options.journal(true);
        }  else if(obj_stoarge_kvp["mongodb_oswc"].compare("replicated") == 0) {
            write_options.acknowledge_level(write_concern::level::k_acknowledged);
        }  else if(obj_stoarge_kvp["mongodb_oswc"].compare("majority") == 0) {
            DBG<<" Write Majority";
            write_options.acknowledge_level(write_concern::level::k_majority);
        }
    }
    AsyncCentralManager::getInstance()->addTimer(this, 1000, 1000);
}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void MongoDBObjectStorageDataAccess::executePush(std::set<DaqBlobShrdPtr>&& _batch_element_to_store) {
    int err = 0;
    try{
        //get client the connection
        auto client = pool_ref.acquire();
        //access to database
        auto db = (*client)[MONGODB_DB_NAME];
        //access a collection
        collection coll_data = db[MONGODB_DAQ_COLL_NAME];
        collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
        mongocxx::options::bulk_write bw;
        bw.write_concern(write_options);
        bw.ordered(true);
        auto bulk_index_write = coll_index.create_bulk_write(bw);
        auto bulk_data_write = coll_data.create_bulk_write(bw);
        
        //create batch insert data
        std::for_each(_batch_element_to_store.begin(),
                      _batch_element_to_store.end(),
                      [&bulk_index_write, &bulk_data_write](const DaqBlobShrdPtr& current_element){
                          //append data to bulk
                          bulk_index_write.append(model::insert_one(current_element->index_document.view()));
                          bulk_data_write.append(model::insert_one(current_element->data_document.view()));
                      });
        
        auto bulk_index_result = bulk_index_write.execute();
        auto bulk_data_result = bulk_data_write.execute();
        
        if(bulk_index_result->inserted_count() != _batch_element_to_store.size() ||
           bulk_data_result->inserted_count() != _batch_element_to_store.size()) {
            ERR << "Data not all data has been isert into database";
        }
    } catch (const bulk_write_exception& e) {
        err =  e.code().value();
        ERR << CHAOS_FORMAT("[%1%] - %2%", %err%e.what());
    }
}

int MongoDBObjectStorageDataAccess::pushObject(const std::string&            key,
                                               const ChaosStringSetConstSPtr meta_tags,
                                               const CDataWrapper&           stored_object) {
    int err = 0;
    bsoncxx::oid new_data_oid;
    DaqBlobShrdPtr current_data = ChaosMakeSharedPtr<DaqBlob>();
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));
    
    //generate shard index information for data and daq
    DaqZonedInfo zone_daq_info = shrd_key_manager.getNewDataPack(key, now_in_ms, stored_object.getBSONRawSize());
    
    //create index
    auto builder_index = builder::basic::document{};
    current_data->index_document.append(kvp("_id", new_data_oid));
    current_data->index_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    current_data->index_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));
    current_data->index_document.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID)));
    current_data->index_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID)));
    //appends tag
    if(meta_tags &&
       meta_tags->size()) {
        auto array_builder = bsoncxx::builder::basic::array{};
        for(auto& it: *meta_tags) {
            array_builder.append(it);
        }
        current_data->index_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), array_builder));
    }
    current_data->index_document.append(bsoncxx::builder::concatenate(zone_daq_info.getIndexDocument().view()));
    
    //create data pack
    auto builder_data = builder::basic::document{};
    current_data->data_document.append(kvp("_id", new_data_oid));
    current_data->data_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    current_data->data_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));
    current_data->data_document.append(bsoncxx::builder::concatenate(zone_daq_info.getDataDocument().view()));
    bsoncxx::document::view view_daq_data((const std::uint8_t*)stored_object.getBSONRawData(), stored_object.getBSONRawSize());
    current_data->data_document.append(kvp(std::string(MONGODB_DAQ_DATA_FIELD), view_daq_data));
    
    //check if we need to push or wait timeout or other incoming data
    curret_batch_size += stored_object.getBSONRawSize();
    DaqBlobSetLWriteLock wl = batch_set.getWriteLockObject();
    batch_set().insert(current_data);
    return err;
}

void MongoDBObjectStorageDataAccess::timeout() {
    if(push_current_step_left) {
        push_current_step_left--;
        return;
    }
    DaqBlobSetLWriteLock wl = batch_set.getWriteLockObject();
    //we can process data
    if(batch_set().size()) {
        //we have data so we can push
        if(current_push_future.valid()) {
            current_push_future.wait();
        }
        current_push_future = std::async(std::launch::async,
                                         &MongoDBObjectStorageDataAccess::executePush,
                                         this,
                                         std::move(batch_set()));
    }
    //reassign multiplier
    push_current_step_left = push_timeout_multiplier;
}

CDWShrdPtr MongoDBObjectStorageDataAccess::getDataByID(mongocxx::database& db,
                                                       const std::string& _id) {
    CDWShrdPtr result;
    int err = 0;
    collection coll_data = db[MONGODB_DAQ_COLL_NAME];
    
    auto opts  = options::find{};
    read_preference read_pref;
    read_pref.mode(read_preference::read_mode::k_secondary_preferred);
    opts.read_preference(read_pref).max_time(std::chrono::milliseconds(common::constants::ObjectStorageTimeoutinMSec));
    
    try {
        bsoncxx::stdx::optional<bsoncxx::document::value> result_find = coll_data.find_one(make_document(kvp("_id", bsoncxx::oid(_id))), opts);
        if(result_find) {
            //we have found data
            auto daq_data_view = result_find->view()[MONGODB_DAQ_DATA_FIELD];
            result = ChaosMakeSharedPtr<CDataWrapper>((const char *)daq_data_view.get_value().get_document().value.data());
        } else {
            //create empty object for not found data
            result = ChaosMakeSharedPtr<CDataWrapper>();
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return result;
}

VectorObject MongoDBObjectStorageDataAccess::getDataByID(mongocxx::database& db,
                                                         const ChaosStringSet& _ids) {
    VectorObject result;
    int err = 0;
    //access a collection
    collection coll_data = db[MONGODB_DAQ_COLL_NAME];
    
    auto opts  = options::find{};
    read_preference read_pref;
    read_pref.mode(read_preference::read_mode::k_secondary_preferred);
    opts.read_preference(read_pref).max_time(std::chrono::milliseconds(common::constants::ObjectStorageTimeoutinMSec));
    opts.sort(make_document(kvp(CHAOS_FORMAT("data.%1%",%chaos::ControlUnitDatapackCommonKey::RUN_ID), 1),
                            kvp(CHAOS_FORMAT("data.%1%",%chaos::DataPackCommonKey::DPCK_SEQ_ID), 1)));
    //    opts.sort(make_document(kvp("_id", 1)));
    try {
        //scan and get data 30 epement at time
        //        int idx = 0;
        ChaosStringSetConstIterator it = _ids.begin();
        //        while(it != _ids.end()) {
        //            idx = 0;
        //            auto builder = builder::basic::document{};
        //            auto array_builder = bsoncxx::builder::basic::array{};
        //            while((it != _ids.end()) &&
        //                (idx <= 30)) {
        //                array_builder.append(bsoncxx::oid(*it));
        //                it++;idx++;
        //            }
        //            builder.append(kvp("_id",  make_document(kvp("$in", array_builder))));
        //            auto cursor = coll_data.find(builder.view(), opts);
        ////            INFO << to_json(builder.view());
        //            for(auto && document : cursor){
        //                auto daq_data_view = document[MONGODB_DAQ_DATA_FIELD];
        //                result.push_back(ChaosMakeSharedPtr<CDataWrapper>((const char *)daq_data_view.get_value().get_document().value.data()));
        //            }
        //        }
        auto builder = builder::basic::document{};
        auto array_builder = bsoncxx::builder::basic::array{};
        while(it != _ids.end()) {
            array_builder.append(bsoncxx::oid(*it));
            it++;
        }
        builder.append(kvp("_id",  make_document(kvp("$in", array_builder))));
        auto cursor = coll_data.find(builder.view(), opts);
        for(auto && document : cursor){
            auto daq_data_view = document[MONGODB_DAQ_DATA_FIELD];
            result.push_back(ChaosMakeSharedPtr<CDataWrapper>((const char *)daq_data_view.get_value().get_document().value.data()));
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return result;
}

int MongoDBObjectStorageDataAccess::getObject(const std::string& key,
                                              const uint64_t& timestamp,
                                              CDWShrdPtr& object_ptr_ref) {
    int err = 0;
    //get client the connection
    auto client = pool_ref.acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
    
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));
    
    try {
        
        auto element = coll_index.find_one(make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key),
                                                         kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson)));
        if(element) {
            //we have found data
            auto id_value = element.value().view()["_id"];
            object_ptr_ref = getDataByID(db,
                                         id_value.get_oid().value.to_string());
        } else {
            //create empty object for not found data
            object_ptr_ref = ChaosMakeSharedPtr<CDataWrapper>();
        }
        
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}

int MongoDBObjectStorageDataAccess::getLastObject(const std::string& key,
                                                  CDWShrdPtr& object_ptr_ref) {
    
    int err = 0;
    //get client the connection
    auto client = pool_ref.acquire();
    
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
    
    try {
        
        auto query = make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
        auto opts  = options::find{};
        opts.sort(make_document(kvp("$natural", -1)));
        
        DEBUG_CODE(DBG<<log_message("getLastObject", "findOne", DATA_ACCESS_LOG_1_ENTRY("Query", bsoncxx::to_json(query.view()))));
        
        auto element = coll_index.find_one(query.view(),opts);
        
        if(element) {
            //we have found data
            auto id_value = element.value().view()["_id"];
            object_ptr_ref = getDataByID(db,
                                         id_value.get_oid().value.to_string());
        } else {
            //create empty object for not found data
            object_ptr_ref = ChaosMakeSharedPtr<CDataWrapper>();
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
    
}


int MongoDBObjectStorageDataAccess::deleteObject(const std::string& key,
                                                 uint64_t start_timestamp,
                                                 uint64_t end_timestamp) {
    int err = 0;
    auto client  = pool_ref.acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
    collection coll_data = db[MONGODB_DAQ_COLL_NAME];
    auto builder = builder::basic::document{};
    
    try{
        
        builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
        if(start_timestamp != 0)
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$gte", b_date(std::chrono::milliseconds(start_timestamp))))));
        if(end_timestamp != 0)
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$lte", b_date(std::chrono::milliseconds(end_timestamp))))));
        
        // array builder
        //TODO: translate BSON("$eq" << mongo::BSONArrayBuilder().arr()) per adesso ho messo 1
        auto empty_array = builder::basic::array{};
        
        builder.append(kvp("$or",make_array(
                                            make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), make_document(kvp("$exists",false)))),
                                            make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), make_document(kvp("$eq",empty_array.view()))))
                                            )
                           )
                       );
        
        DEBUG_CODE(DBG<<log_message(__func__,
                                    "delete",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            bsoncxx::to_json(builder.view()))));
        
        
        // delete
        write_concern wc;
        wc.acknowledge_level(write_concern::level::k_unacknowledged);
        auto dop = options::delete_options();
        dop.write_concern(wc);
        coll_data.delete_many(builder.view(), dop);
        coll_index.delete_many(builder.view(), dop);
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    
    return err;
    
}

int MongoDBObjectStorageDataAccess::findObject(const std::string&                                          key,
                                               const ChaosStringSet&                                       meta_tags,
                                               const uint64_t                                              timestamp_from,
                                               const uint64_t                                              timestamp_to,
                                               const uint32_t                                              page_len,
                                               abstraction::VectorObject&                                  found_object_page,
                                               common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
    int err = 0;
    auto client  = pool_ref.acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
    auto builder = builder::basic::document{};
    auto time_builder = builder::basic::document{};
    try{
        bool reverse_order = false;
        //we have the intervall
        reverse_order = timestamp_from>timestamp_to;
        builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
        if(reverse_order == false) {
            time_builder.append(kvp("$gte", b_date(std::chrono::milliseconds(timestamp_from))));
            time_builder.append(kvp("$lte", b_date(std::chrono::milliseconds(timestamp_to))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), time_builder.view()));
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), make_document(kvp("$gte", last_record_found_seq.run_id))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), make_document(kvp("$gte",  last_record_found_seq.datapack_counter ))));
        } else {
            time_builder.append(kvp("$lte", b_date(std::chrono::milliseconds(timestamp_from))));
            time_builder.append(kvp("$gte", b_date(std::chrono::milliseconds(timestamp_to))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), time_builder.view()));
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), make_document(kvp("$lte", last_record_found_seq.run_id))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), make_document(kvp("$lte",  last_record_found_seq.datapack_counter ))));
        }
        
        if(meta_tags.size()) {
            auto array_builder = bsoncxx::builder::basic::array{};
            for(auto& it: meta_tags) {
                array_builder.append(it);
            }
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), make_document(kvp("$all", array_builder))));
        }
        //create find option
        auto opts  = options::find{};
        //set page len
        opts.limit(page_len);
        //set read form secondary
        read_preference secondary;
        secondary.mode(read_preference::read_mode::k_secondary_preferred);
        opts.read_preference(secondary).max_time(std::chrono::milliseconds(common::constants::ObjectStorageTimeoutinMSec));
        opts.read_preference(secondary).batch_size(30);
        if(reverse_order) {
            opts.sort(make_document(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), -1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), -1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), -1)));
        } else {
            opts.sort(make_document(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), 1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), 1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP),  1)));
        }
        
        DEBUG_CODE(DBG<<log_message("findObject", "find", DATA_ACCESS_LOG_1_ENTRY("Query", bsoncxx::to_json(builder.view()))));
        
        ChaosStringSet foud_ids;
        auto cursor = coll_index.find(builder.view(),opts);
        for(auto && document : cursor){
            auto id_value = document["_id"];
            if(id_value.type() != bsoncxx::type::k_oid){
                continue;
            }
            //            foud_ids.insert(id_value.get_oid().value.to_string());
            found_object_page.push_back(getDataByID(db,
                                                    id_value.get_oid().value.to_string()));
            last_record_found_seq.run_id = document[chaos::ControlUnitDatapackCommonKey::RUN_ID].get_int64();
            last_record_found_seq.datapack_counter = document[chaos::DataPackCommonKey::DPCK_SEQ_ID].get_int64();
        }
        //        found_object_page = getDataByID(db, foud_ids);
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}

//inhertied method
int MongoDBObjectStorageDataAccess::findObjectIndex(const DataSearch& search,
                                                    VectorObject& found_object_page,
                                                    chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
    int err = 0;
    auto client  = pool_ref.acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
    auto builder = builder::basic::document{};
    auto time_builder = builder::basic::document{};
    auto builder_project = builder::basic::document{};
    try{
        
        bool reverse_order = false;
        //we have the intervall
        reverse_order = search.timestamp_from>search.timestamp_to;
        builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), search.key));
        if(reverse_order == false) {
            time_builder.append(kvp("$gte", b_date(std::chrono::milliseconds(search.timestamp_from))));
            time_builder.append(kvp("$lte", b_date(std::chrono::milliseconds(search.timestamp_to))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), time_builder.view()));
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), make_document(kvp("$gte", last_record_found_seq.run_id))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), make_document(kvp("$gte",  last_record_found_seq.datapack_counter ))));
        }else{
            time_builder.append(kvp("$lte", b_date(std::chrono::milliseconds(search.timestamp_from))));
            time_builder.append(kvp("$gte", b_date(std::chrono::milliseconds(search.timestamp_to))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), time_builder.view()));
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), make_document(kvp("$lte", last_record_found_seq.run_id))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), make_document(kvp("$lte",  last_record_found_seq.datapack_counter ))));
        }
        if(search.meta_tags.size()) {
            auto array_builder = bsoncxx::builder::basic::array{};
            for(auto& it: search.meta_tags) {
                array_builder.append(it);
            }
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), make_document(kvp("$all", array_builder))));
        }
        //create find option
        auto opts  = options::find{};
        //set page len
        opts.limit(search.page_len);
        //not return the data field
        builder_project.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), 1));
        builder_project.append(kvp("zone_key", 1));
        builder_project.append(kvp("shard_key", 1));
        builder_project.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), 1));
        builder_project.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), 1));
        opts.projection(builder_project.view());
        
        //set read form secondary
        read_preference secondary;
        secondary.mode(read_preference::read_mode::k_secondary_preferred);
        opts.read_preference(secondary).max_time(std::chrono::milliseconds(common::constants::ObjectStorageTimeoutinMSec));
        if(reverse_order) {
            opts.sort(make_document(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), -1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), -1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), -1)));
        } else {
            opts.sort(make_document(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), 1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), 1),
                                    kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), 1)));
        }
        
        DEBUG_CODE(DBG<<log_message("findObject", "find", DATA_ACCESS_LOG_2_ENTRY("Query",
                                                                                  "Projection",
                                                                                  bsoncxx::to_json(builder.view()),
                                                                                  bsoncxx::to_json(opts.projection().value().view()))));
        auto cursor = coll_index.find(builder.view(),opts);
        //get the size
        
        //if(size) {
        for(auto && document : cursor){
            auto new_obj = ChaosMakeSharedPtr<CDataWrapper>();
            new_obj->append(chaos::DataPackCommonKey::DPCK_DEVICE_ID, document[chaos::DataPackCommonKey::DPCK_DEVICE_ID].get_utf8().value.to_string());
            new_obj->append("zone_key", document["zone_key"].get_utf8().value.to_string());
            new_obj->append("shard_key", document["shard_key"].get_int64());
            new_obj->append(chaos::ControlUnitDatapackCommonKey::RUN_ID, document[chaos::ControlUnitDatapackCommonKey::RUN_ID].get_int64());
            new_obj->append(chaos::DataPackCommonKey::DPCK_SEQ_ID, document[chaos::DataPackCommonKey::DPCK_SEQ_ID].get_int64());
            found_object_page.push_back(new_obj);
            last_record_found_seq.run_id = document[chaos::ControlUnitDatapackCommonKey::RUN_ID].get_int64();
            last_record_found_seq.datapack_counter = document[chaos::DataPackCommonKey::DPCK_SEQ_ID].get_int64();
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}

//inhertied method
int MongoDBObjectStorageDataAccess::getObjectByIndex(const CDWShrdPtr& index,
                                                     CDWShrdPtr& found_object) {
    int err = 0;
    auto client  = pool_ref.acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll = db[MONGODB_DAQ_COLL_NAME];
    auto opts  = options::find{};
    opts.projection(bsoncxx::builder::basic::make_document(kvp(MONGODB_DAQ_DATA_FIELD, 1)));
    
    const std::string run_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::ControlUnitDatapackCommonKey::RUN_ID);
    const std::string seq_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID);
    try{
        auto builder = builder::basic::document{};
        builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), index->getStringValue(chaos::DataPackCommonKey::DPCK_DEVICE_ID)));
        builder.append(kvp(std::string("zone_key"), index->getStringValue("zone_key")));
        builder.append(kvp(std::string("shard_key"), index->getInt64Value("shard_key")));
        builder.append(kvp(run_key, index->getInt64Value("cudk_run_id")));
        builder.append(kvp(seq_key, index->getInt64Value("dpck_seq_id")));
        
        DEBUG_CODE(DBG<<log_message("getObjectByIndex", "find", DATA_ACCESS_LOG_1_ENTRY("Query",
                                                                                        bsoncxx::to_json(builder.view()))));
        
        auto result = coll.find_one(builder.view(), opts);
        if(result) {
            //we have found data
            auto result_view = result.value().view()[MONGODB_DAQ_DATA_FIELD];
            found_object = ChaosMakeSharedPtr<CDataWrapper>((const char *)result_view.raw());
        } else {
            //create empty object for not found data
            found_object = ChaosMakeSharedPtr<CDataWrapper>();
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}

int MongoDBObjectStorageDataAccess::countObject(const std::string& key,
                                                const uint64_t timestamp_from,
                                                const uint64_t timestamp_to,
                                                const uint64_t& object_count) {
    int err = 0;
    auto client  = pool_ref.acquire();
    return err;
}

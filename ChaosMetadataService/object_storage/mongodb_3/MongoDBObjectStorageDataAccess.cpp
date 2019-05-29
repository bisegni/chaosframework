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

#define MONGODB_DB_NAME                 "chaos"
#define MONGODB_DAQ_COLL_NAME           "daq"
#define MONGODB_DAQ_INDEX_COLL_NAME     "daq_index"
#define MONGODB_DAQ_DATA_FIELD          "data"
#define DEFAULT_QUANTIZATION            100

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
using namespace chaos::common::batch_command;
using namespace chaos::common::direct_io::channel::opcode_headers;

//using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::metadata_service::object_storage::mongodb_3;
using namespace chaos::metadata_service::object_storage::abstraction;


MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(pool& _pool_ref):
pool_ref(_pool_ref){
    //get client the connection
    auto client = pool_ref.acquire();
    
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    
    
    {//index for sharding on index collection
        auto index_builder = builder::basic::document{};
        mongocxx::options::index index_options{};
        index_builder.append(kvp("zone_key", 1));
        index_builder.append(kvp("shard_key", 1));
        index_options.name("shard_index");
        db[MONGODB_DAQ_INDEX_COLL_NAME].create_index(index_builder.view(), index_options);
    }

    {//index for search on index collection
        auto index_builder = builder::basic::document{};
        mongocxx::options::index index_options{};
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), 1));
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), 1));
        index_builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), 1));
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), 1));
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), 1));
        index_options.name("paged_daq_seq_search_index");
        db[MONGODB_DAQ_INDEX_COLL_NAME].create_index(index_builder.view(), index_options);
    }

    {//index for sharding on data collection
        auto index_builder = builder::basic::document{};
        mongocxx::options::index index_options{};
        index_builder.append(kvp("zone_key", 1));
        index_builder.append(kvp("shard_key", 1));
        index_options.name("shard_index");
        db[MONGODB_DAQ_COLL_NAME].create_index(index_builder.view(), index_options);
    }

    {//index for search on data collection
        const std::string run_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::ControlUnitDatapackCommonKey::RUN_ID);
        const std::string seq_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID);
        auto index_builder = builder::basic::document{};
        mongocxx::options::index index_options{};
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), 1));
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), 1));
        index_builder.append(kvp(run_key, 1));
        index_builder.append(kvp(seq_key, 1));
        index_builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), 1));
        index_options.name("paged_daq_seq_search_index");
        db[MONGODB_DAQ_COLL_NAME].create_index(index_builder.view(), index_options);
    }
}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {}

int MongoDBObjectStorageDataAccess::pushObject(const std::string&            key,
                                               const ChaosStringSetConstSPtr meta_tags,
                                               const CDataWrapper&           stored_object) {
    int err = 0;
    bsoncxx::oid new_data_oid;
    //get client the connection
    auto client = pool_ref.acquire();
    
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_data = db[MONGODB_DAQ_COLL_NAME];
    collection coll_index = db[MONGODB_DAQ_INDEX_COLL_NAME];
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));
    
    //generate shard index
    bsoncxx::builder::basic::document zone_pack = shrd_key_manager.getNewDataPack(key, now_in_ms, stored_object.getBSONRawSize());
    
    //create index
    auto builder_index = builder::basic::document{};
    builder_index.append(kvp("_id", new_data_oid));
    builder_index.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    builder_index.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));
    builder_index.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID)));
    builder_index.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID)));
    //appends tag
    if(meta_tags &&
       meta_tags->size()) {
        auto array_builder = bsoncxx::builder::basic::array{};
        for(auto& it: *meta_tags) {
            array_builder.append(it);
        }
        builder_index.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), array_builder));
    }
    builder_index.append(bsoncxx::builder::concatenate(zone_pack.view()));
    
    //create data pack
    auto builder_data = builder::basic::document{};
    builder_data.append(kvp("_id", new_data_oid));
    builder_data.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    builder_data.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));
    builder_data.append(bsoncxx::builder::concatenate(zone_pack.view()));
    bsoncxx::document::view view_daq_data((const std::uint8_t*)stored_object.getBSONRawData(), stored_object.getBSONRawSize());
    builder_data.append(kvp(std::string(MONGODB_DAQ_DATA_FIELD), view_daq_data));
    try {
        auto res_idx = coll_index.insert_one(builder_index.view());
        if(res_idx &&
           res_idx.value().result().inserted_count() == 1) {
            auto res_data = coll_data.insert_one(builder_data.view());
            if(!res_data ||
               res_data.value().result().inserted_count() != 1) {
                ERR << "Error storing data";
            }
        } else {
            ERR << "Error storing index data";
        }
    } catch (const bulk_write_exception& e) {
        err =  e.code().value();
        ERR << CHAOS_FORMAT("[%1%] - %2%", %err%e.what());
    }
    return err;
}

CDWShrdPtr MongoDBObjectStorageDataAccess::getDataByID(mongocxx::pool::entry& client,
                                                       const std::string& _id) {
    CDWShrdPtr result;
    int err = 0;
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll_data = db[MONGODB_DAQ_COLL_NAME];
    
    auto opts  = options::find{};
    read_preference read_pref;
    read_pref.mode(read_preference::read_mode::k_secondary_preferred);
    opts.read_preference(read_pref).max_time(std::chrono::milliseconds(common::constants::ObjectStorageTimeoutinMSec));
    
    try {
        auto result_find = coll_data.find_one(make_document(kvp("_id", bsoncxx::oid(_id))), opts);
        if(result_find) {
            //we have found data
            auto result_view = result_find.value().view()[MONGODB_DAQ_DATA_FIELD];
            result = ChaosMakeSharedPtr<CDataWrapper>((const char *)result_view.raw());
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
            object_ptr_ref = getDataByID(client,
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
            object_ptr_ref = getDataByID(client,
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
        auto cursor = coll_index.find(builder.view(),opts);
        for(auto && document : cursor){
            auto id_value = document["_id"];
            if(id_value.type() != bsoncxx::type::k_oid){
                continue;
            }
            found_object_page.push_back(getDataByID(client,
                                                    id_value.get_oid().value.to_string()));
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

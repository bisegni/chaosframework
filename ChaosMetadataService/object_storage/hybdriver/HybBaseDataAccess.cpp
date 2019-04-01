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

#include "HybBaseDataAccess.h"
#include "../../ChaosMetadataService.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>

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

#include <boost/lexical_cast.hpp>

//using namespace chaos::data_service::object_storage::mongodb;

#define INFO INFO_LOG(HybBaseDataAccess)
#define DBG  DBG_LOG(HybBaseDataAccess)
#define ERR  ERR_LOG(HybBaseDataAccess)

#define MONGODB_DB_NAME             "chaos"
#define MONGODB_DAQ_COLL_NAME       "daq"
#define MONGODB_DAQ_DATA_FIELD      "data"
#define DEFAULT_QUANTIZATION        100

#define DEFAULT_BATCH_SIZE          10
#define DEFAULT_BATCH_TIMEOUT_MS    1000

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_date;

using namespace mongocxx;
using namespace bsoncxx;
using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::direct_io::channel::opcode_headers;

using namespace chaos::metadata_service::object_storage::hybdriver;
using namespace chaos::metadata_service::object_storage::abstraction;

HybBaseDataAccess::HybBaseDataAccess():
batch_timeout(DEFAULT_BATCH_TIMEOUT_MS),
blob_set_uptr(new std::set<DaqBlobSPtr>()){
    MapKVP& obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    try{
        batch_size = boost::lexical_cast<CUInt32>(obj_stoarge_kvp["batch_size"]);
    } catch(...) {
        batch_size = DEFAULT_BATCH_SIZE;
    }
    next_timeout = TimingUtil::getTimeStamp() + batch_timeout;
}
HybBaseDataAccess::~HybBaseDataAccess(){}


void HybBaseDataAccess::executePush(ChaosUniquePtr<std::set<DaqBlobSPtr>> _blob_set_uptr) {
    int err = 0;
    try{
        if(!_blob_set_uptr.get()) return;
        //get client the connection
        auto client = pool_ref->acquire();
        auto db = (*client)[MONGODB_DB_NAME];
        collection coll = db[MONGODB_DAQ_COLL_NAME];
        auto bulk_write = coll.create_bulk_write();
        
        //create batch insert data
        std::for_each(_blob_set_uptr->begin(), _blob_set_uptr->end(), [&bulk_write, this](const DaqBlobSPtr& blob){
            int err = 0;
            //insert data operation is demanded to sublcass
            if((err = storeData(*blob))) {
                //errore in pushing, data need to bedeleted
                ERR << CHAOS_FORMAT("Error %1% during data storage", %err);
            } else {
                //append data to bulk
                bulk_write.append(model::insert_one(blob->mongo_document.view()));
            }
        });
        auto bulk_result = coll.bulk_write(bulk_write);
        
        if(bulk_result->inserted_count() != _blob_set_uptr->size()) {
            ERR << "Data not inserted";
        }
    } catch (const bulk_write_exception& e) {
        err =  e.code().value();
        ERR << CHAOS_FORMAT("[%1%] - %2%", %err%e.what());
    }
}

int HybBaseDataAccess::pushObject(const std::string&            key,
                                  const ChaosStringSetConstSPtr meta_tags,
                                  const CDataWrapper&           stored_object) {
    int err = 0;
    DaqBlobSPtr daq_blob = std::make_shared<DaqBlob>();
    int64_t shard_value;
    if(stored_object.hasKey(chaos::ControlUnitDatapackCommonKey::RUN_ID) == false ||
       stored_object.hasKey(chaos::DataPackCommonKey::DPCK_SEQ_ID) == false) {
        return -1;
    }
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));
    
    daq_blob->mongo_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    daq_blob->mongo_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));
    daq_blob->mongo_document.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID)));
    daq_blob->mongo_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID)));
    //appends tag
    using bsoncxx::builder::basic::sub_array;
    if(meta_tags && meta_tags->size()) {
        daq_blob->mongo_document.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), [](sub_array subarr, const ChaosStringSetConstSPtr meta_tags) {
            for(ChaosStringSetConstIterator it = meta_tags->begin(),
                end = meta_tags->end();
                it != end;
                it++){
                subarr.append(*it);
            }
        }));
    }
    
    bsoncxx::builder::basic::document zone_pack = shrd_key_manager.getNewDataPack(key, now_in_ms, stored_object.getBSONRawSize(), shard_value);
    daq_blob->mongo_document.append(bsoncxx::builder::concatenate(zone_pack.view()));
    
    //create data blob
    daq_blob->index = (DaqIndex){key,
        shard_value,
        stored_object.getInt64Value(chaos::ControlUnitDatapackCommonKey::RUN_ID),
        stored_object.getInt64Value(chaos::DataPackCommonKey::DPCK_SEQ_ID)};
    daq_blob->data_blob = stored_object.getBSONDataBuffer();
    
    //add blob to set
    blob_set_uptr->insert(daq_blob);
    if(blob_set_uptr->size() >= batch_size ||
       now_in_ms >= next_timeout) {
        if(current_push_future.valid()) {
            current_push_future.wait();
        }
        current_push_future = std::async(std::launch::async,
                                         &HybBaseDataAccess::executePush,
                                         this,
                                         std::move(blob_set_uptr));
        blob_set_uptr.reset(new std::set<DaqBlobSPtr>());
        //reset timeout endtime
        next_timeout = now_in_ms + batch_timeout;
    }
    
    return err;
}


int HybBaseDataAccess::getObject(const std::string& key,
                                 const uint64_t& timestamp,
                                 CDWShrdPtr& object_ptr_ref) {
    
    int err = 0;
    CDWUniquePtr found_element;
    //get client the connection
    auto client = pool_ref->acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll = db[MONGODB_DAQ_COLL_NAME];
    
    auto target_ms = b_date(std::chrono::milliseconds(timestamp));
    
    try {
        auto element = coll.find_one(make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key),
                                                   kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), target_ms)));
        
        if(element){
            auto view = element->view();
            auto did = view[chaos::DataPackCommonKey::DPCK_DEVICE_ID];
            auto shard_key = view["shard_key"];
            auto run_id = view[chaos::ControlUnitDatapackCommonKey::RUN_ID];
            auto seq_id = view[chaos::DataPackCommonKey::DPCK_SEQ_ID];
            DaqIndex index = {did.get_utf8().value.to_string(),
                shard_key.get_int64(),
                run_id.get_int64(),
                seq_id.get_int64()};
            if((err = retrieveData(index,
                                   found_element))) {
                ERR << CHAOS_FORMAT("Error %1% retriving data from storage layer", %err);
            } else {
                object_ptr_ref = CDWShrdPtr(found_element.release());
            }
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}

int HybBaseDataAccess::getLastObject(const std::string& key,
                                     CDWShrdPtr& object_ptr_ref) {
    int err = 0;
    CDWUniquePtr found_element;
    //get client the connection
    auto client = pool_ref->acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll = db[MONGODB_DAQ_COLL_NAME];
    try {
        auto query = make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
        auto opts  = options::find{};
        opts.sort(make_document(kvp("$natural", -1)));
        
        DEBUG_CODE(DBG<<log_message("getLastObject", "findOne", DATA_ACCESS_LOG_1_ENTRY("Query", bsoncxx::to_json(query.view()))));
        
        auto bson_element = coll.find_one(query.view(),opts);
        
        if(bson_element){
            auto view = bson_element->view();
            auto did = view[chaos::DataPackCommonKey::DPCK_DEVICE_ID];
            auto shard_key = view["shard_key"];
            auto run_id = view[chaos::ControlUnitDatapackCommonKey::RUN_ID];
            auto seq_id = view[chaos::DataPackCommonKey::DPCK_SEQ_ID];
            DaqIndex index = {did.get_utf8().value.to_string(),
                shard_key.get_int64(),
                run_id.get_int64(),
                seq_id.get_int64()};
            if((err = retrieveData(index,
                                   found_element))) {
                ERR << CHAOS_FORMAT("Error %1% retriving data from storage layer", %err);
            } else {
                object_ptr_ref = CDWShrdPtr(found_element.release());
            }
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}


int HybBaseDataAccess::deleteObject(const std::string& key,
                                    uint64_t start_timestamp,
                                    uint64_t end_timestamp) {
    int err = 0;
    auto client  = pool_ref->acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll = db[MONGODB_DAQ_COLL_NAME];
    
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
        auto result= coll.delete_many(builder.view(), dop);
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
    
}

int HybBaseDataAccess::findObject(const std::string&                                          key,
                                  const ChaosStringSet&                                       meta_tags,
                                  const uint64_t                                              timestamp_from,
                                  const uint64_t                                              timestamp_to,
                                  const uint32_t                                              page_len,
                                  abstraction::VectorObject&                                  found_object_page,
                                  common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
    int err = 0;
    auto client  = pool_ref->acquire();
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    collection coll = db[MONGODB_DAQ_COLL_NAME];
    auto builder = builder::basic::document{};
    try{
        
        bool reverse_order = false;
        //we have the intervall
        reverse_order = timestamp_from>timestamp_to;
        if(reverse_order == false) {
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$gte", b_date(std::chrono::milliseconds(timestamp_from))))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$lte", b_date(std::chrono::milliseconds(timestamp_to))))));
            
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), make_document(kvp("$gte", last_record_found_seq.run_id))));
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID), make_document(kvp("$gte", last_record_found_seq.datapack_counter ))));
        }else{
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$lte", b_date(std::chrono::milliseconds(timestamp_from))))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$gte", b_date(std::chrono::milliseconds(timestamp_to))))));
            builder.append(kvp(std::string(chaos::ControlUnitDatapackCommonKey::RUN_ID),     make_document(kvp("$lte", last_record_found_seq.run_id))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_SEQ_ID), make_document(kvp("$lte",  last_record_found_seq.datapack_counter ))));
        }
        
        if(meta_tags.size()) {
            auto array_builder = bsoncxx::builder::basic::array{};
            for(auto& it: meta_tags) {
                array_builder.append(it);
            }
            builder.append(kvp("$all",make_array(array_builder)));
        }
        //create find option
        auto opts  = options::find{};
        //set page len
        opts.limit(page_len);
        //set read form secondary
        read_preference secondary;
        secondary.mode(read_preference::read_mode::k_secondary);
        opts.read_preference(secondary).max_time(std::chrono::milliseconds(common::constants::ObjectStorageTimeoutinMSec));
        if(reverse_order) {
            opts.sort(make_document(kvp("counter_key", -1),kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), -1)));
        } else {
            opts.sort(make_document(kvp("counter_key",  1),kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP),  1)));
        }
        
        DEBUG_CODE(DBG<<log_message("findObject", "find", DATA_ACCESS_LOG_1_ENTRY("Query", bsoncxx::to_json(builder.view()))));
        auto cursor = coll.find(builder.view(),opts);
        //if(size) {
        CDWUniquePtr found_element;
        for(auto && document : cursor){
            last_record_found_seq.run_id = document[chaos::ControlUnitDatapackCommonKey::RUN_ID].get_int64();
            last_record_found_seq.datapack_counter = document[chaos::ControlUnitDatapackCommonKey::RUN_ID].get_int64();
            auto did = document[chaos::DataPackCommonKey::DPCK_DEVICE_ID];
            auto shard_key = document["shard_key"];
            auto run_id = document[chaos::ControlUnitDatapackCommonKey::RUN_ID];
            auto seq_id = document[chaos::DataPackCommonKey::DPCK_SEQ_ID];
            DaqIndex index = {did.get_utf8().value.to_string(),
                shard_key.get_int64(),
                run_id.get_int64(),
                seq_id.get_int64()};
            if((err = retrieveData(index,
                                   found_element))) {
                ERR << CHAOS_FORMAT("Error %1% retriving data from storage layer", %err);
            } else {
                found_object_page.push_back(CDWShrdPtr(found_element.release()));
            }
            
        }
    } catch (const mongocxx::exception &e) {
        ERR << e.what();
        err = e.code().value();
    }
    return err;
}

//inhertied method
int HybBaseDataAccess::findObjectIndex(const DataSearch& search,
                                       VectorObject& found_object_page,
                                       chaos::common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
    return -1;
}

//inhertied method
int HybBaseDataAccess::getObjectByIndex(const VectorObject& search,
                                        VectorObject& found_object_page) {
    return -1;
}

int HybBaseDataAccess::countObject(const std::string& key,
                                   const uint64_t timestamp_from,
                                   const uint64_t timestamp_to,
                                   const uint64_t& object_count) {
    int err = 0;
    auto client  = pool_ref->acquire();
    
    return err;
}

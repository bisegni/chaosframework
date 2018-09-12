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

#define MONGODB_DB_NAME       "chaos"
#define MONGODB_DAQ_COLL_NAME       "daq"
#define MONGODB_DAQ_DATA_FIELD      "data"
#define DEFAULT_QUANTIZATION        100

using bsoncxx::builder::basic::kvp;
using bsoncxx::builder::basic::make_array;
using bsoncxx::builder::basic::make_document;
using bsoncxx::types::b_date;


using namespace bsoncxx;
using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::direct_io::channel::opcode_headers;

//using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::data_service::object_storage::mongodb_3;
using namespace chaos::data_service::object_storage::abstraction;


MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(mongocxx::pool& _pool_ref):
pool_ref(_pool_ref){}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {}

int MongoDBObjectStorageDataAccess::pushObject(const std::string&            key,
                                               const ChaosStringSetConstSPtr meta_tags,
                                               const CDataWrapper&           stored_object) {
    int err = 0;
    //get client the connection
    auto client = pool_ref.acquire();
    
    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    mongocxx::collection coll = db[MONGODB_DAQ_COLL_NAME];
    
    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));
    
    // basic::document builds a BSON document.
    auto builder = builder::basic::document{};
    builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));
    //appends tag
    using bsoncxx::builder::basic::sub_array;
    if(meta_tags && meta_tags->size()) {
        builder.append(mongocxx::kvp(std::string(chaos::DataPackCommonKey::DPCK_DATASET_TAGS), [](sub_array subarr, const ChaosStringSetConstSPtr meta_tags) {
            for(ChaosStringSetConstIterator it = meta_tags->begin(),
                end = meta_tags->end();
                it != end;
                it++){
                subarr.append(*it);
            }
        }));
    }
    
    bsoncxx::builder::basic::document zone_pack = shrd_key_manager.getNewDataPack(key, now_in_ms, stored_object.getBSONRawSize());
    builder.append(bsoncxx::builder::concatenate(zone_pack.view()));
    
    bsoncxx::document::view view_daq_data((const std::uint8_t*)stored_object.getBSONRawData(), stored_object.getBSONRawSize());
    builder.append(kvp(std::string(MONGODB_DAQ_DATA_FIELD), view_daq_data));
    try {
        coll.insert_one(builder.view());
    } catch (const mongocxx::bulk_write_exception& e) {
        err =  e.code().value();
        ERR << CHAOS_FORMAT("[%1%] - %2%", %err%e.what());
    }
    return err;
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
    mongocxx::collection coll = db[MONGODB_DAQ_COLL_NAME];

    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));

    try {

        auto cursor = coll.find(make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key), 
                                              kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson)));

        bool found = false;
        for(auto && document : cursor){
            found = true;
            if(document[MONGODB_DAQ_DATA_FIELD]){
                auto json_string = bsoncxx::to_json(document);
                object_ptr_ref.reset(new CDataWrapper(json_string.data()));
            }
        }

        if(!found){
            DBG << "No data has been found";
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
    mongocxx::collection coll = db[MONGODB_DAQ_COLL_NAME];

    try {

        auto query = make_document(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
        auto opts  = mongocxx::options::find{};
        opts.sort(make_document(kvp("$natural", -1)));

        DEBUG_CODE(DBG<<log_message("getLastObject", "findOne", DATA_ACCESS_LOG_1_ENTRY("Query", bsoncxx::to_json(query.view()))));

        auto cursor = coll.find(query.view(),opts);

        bool found = false;
        for(auto && document : cursor){
            found = true;
            if(document[MONGODB_DAQ_DATA_FIELD]){
                auto json_string = bsoncxx::to_json(document);
                object_ptr_ref.reset(new CDataWrapper(json_string.data()));
            }
        }

        if(!found){
            DBG << "No data has been found";
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
    mongocxx::collection coll = db[MONGODB_DAQ_COLL_NAME];
    //delete object

    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
    auto now_in_ms_bson = b_date(std::chrono::milliseconds(now_in_ms));

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
        
        DEBUG_CODE(DBG<<log_message("deleteObject", "delete", DATA_ACCESS_LOG_1_ENTRY("Query", bsoncxx::to_json(builder.view()))));


        // delete
        auto result= coll.delete_many(builder.view());

        if(result){
            ERR << CHAOS_FORMAT("Error erasing stored object data for key %1% from %2% to %3%", %key%start_timestamp%end_timestamp);
        }

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
    mongocxx::collection coll = db[MONGODB_DAQ_COLL_NAME];
    auto builder = builder::basic::document{};

    try{

        bool reverse_order = false;
        const std::string run_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::ControlUnitDatapackCommonKey::RUN_ID);
        const std::string counter_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID);
        //we have the intervall
        reverse_order = timestamp_from>timestamp_to;

        if(reverse_order == false) {
            
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$gte", b_date(std::chrono::milliseconds(timestamp_from))))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$lte", b_date(std::chrono::milliseconds(timestamp_to))))));

            builder.append(kvp(run_key,     make_document(kvp("$gte", last_record_found_seq.run_id))));
            builder.append(kvp(counter_key, make_document(kvp("$gte", last_record_found_seq.datapack_counter ))));    

        }else{

            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$lte", b_date(std::chrono::milliseconds(timestamp_from))))));
            builder.append(kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), make_document(kvp("$gte", b_date(std::chrono::milliseconds(timestamp_to))))));
            builder.append(kvp(run_key,     make_document(kvp("$lte", last_record_found_seq.run_id))));
            builder.append(kvp(counter_key, make_document(kvp("$lte",  last_record_found_seq.datapack_counter )))); 

        }

        if(meta_tags.size()) {
            auto array_builder = bsoncxx::builder::basic::array{};
            for(auto& it: meta_tags) {
                array_builder.append(it);
            }
            builder.append(kvp("$all",make_array(array_builder)));
        }

        auto opts  = mongocxx::options::find{};
        //auto cursor = coll.find(query.view(),opts);

        if(reverse_order) {
            opts.sort(make_document(kvp("counter_key", -1),kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), -1)));
        } else {
            opts.sort(make_document(kvp("counter_key",  1),kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP),  1)));
        }

        auto cursor = coll.find(builder.view(),opts);

        //get the size
        uint64_t size  = (uint64_t)std::distance(cursor.begin(),cursor.end());
        uint64_t this_index = 1; 

        bool found = false;

        for(auto && document : cursor){
            found = true;
            auto json_string = bsoncxx::to_json(document);
            CDWShrdPtr new_obj(new CDataWrapper(json_string.data()));
            found_object_page.push_back(new_obj);

            if(this_index == size){

                //TODO: translate non sono sicuro del k_int64 (prima era numeric)
                
                if(document[run_key].type() == bsoncxx::type::k_int32){
                    last_record_found_seq.run_id = document[run_key].get_int64(); 
                }else{
                    last_record_found_seq.run_id = document[run_key].get_double(); 
                }

                if(document[counter_key].type() == bsoncxx::type::k_int32){
                    last_record_found_seq.run_id = document[counter_key].get_int64(); 
                }else{
                    last_record_found_seq.run_id = document[counter_key].get_double(); 
                }
                
            }

        }

        if(!found){
            DBG << "No data has been found";
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

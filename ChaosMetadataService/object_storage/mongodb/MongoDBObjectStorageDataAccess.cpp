/*
 * Copyright 2012, 2017 INFN
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

using namespace chaos::data_service::object_storage::mongodb;


#define INFO INFO_LOG(MongoDBObjectStorageDataAccess)
#define DBG  DBG_LOG(MongoDBObjectStorageDataAccess)
#define ERR  ERR_LOG(MongoDBObjectStorageDataAccess)

#define MONGODB_DAQ_COLL_NAME       "daq"
#define MONGODB_DAQ_DATA_FIELD      "data"
#define DEFAULT_QUANTIZATION        100

using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::common::direct_io::channel::opcode_headers;

using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::data_service::object_storage::mongodb;
using namespace chaos::data_service::object_storage::abstraction;

MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection),
storage_write_concern(&mongo::WriteConcern::unacknowledged) {
    shrd_key_manager.setZoneAlias(metadata_service::ChaosMetadataService::getInstance()->setting.ha_zone_name);
    obj_stoarge_kvp = metadata_service::ChaosMetadataService::getInstance()->setting.object_storage_setting.key_value_custom_param;
    if(obj_stoarge_kvp.count("mongodb_oswc")) {
        //set the custom write concern
        INFO << CHAOS_FORMAT("Set MongoDB object storage write concern to %1%", %obj_stoarge_kvp["mongodb_oswc"]);
        if(obj_stoarge_kvp["mongodb_oswc"].compare("unacknowledged") == 0) {
            storage_write_concern = &mongo::WriteConcern::unacknowledged;
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("acknowledged") == 0) {
            storage_write_concern = &mongo::WriteConcern::acknowledged;
        } else if(obj_stoarge_kvp["mongodb_oswc"].compare("journaled") == 0) {
            storage_write_concern = &mongo::WriteConcern::journaled;
        }  else if(obj_stoarge_kvp["mongodb_oswc"].compare("replicated") == 0) {
            storage_write_concern = &mongo::WriteConcern::replicated;
        }  else if(obj_stoarge_kvp["mongodb_oswc"].compare("majority") == 0) {
            storage_write_concern = &mongo::WriteConcern::majority;
        } else {
            ERR << CHAOS_FORMAT("Unrecognized value for parameter mongodb_oswc[%1%]", %obj_stoarge_kvp["mongodb_oswc"]);
        }
    }
}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {}

int MongoDBObjectStorageDataAccess::pushObject(const std::string& key,
                                               const CDataWrapper& stored_object) {
    int err = 0;
    try {
        int buffer_size = 0;
        const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;
        const char *buffer = stored_object.getBSONRawData(buffer_size);
        mongo::BSONObjBuilder insert_builder;
        insert_builder << chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
        chaos::DataPackCommonKey::DPCK_TIMESTAMP << mongo::Date_t(now_in_ms);
        //add zone sharding information
        mongo::BSONObj zone_pack = shrd_key_manager.getNewDataPack(key,
                                                                   now_in_ms,
                                                                   buffer_size);
        insert_builder.appendElements(zone_pack);
        
        //add data;
        insert_builder << MONGODB_DAQ_DATA_FIELD << mongo::BSONObj(buffer);
        
        mongo::BSONObj i = insert_builder.obj();
        
        //insert
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                                     i,
                                     storage_write_concern))){
            ERR << "Error pushing object";
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBObjectStorageDataAccess::getObject(const std::string& key,
                                              const uint64_t& timestamp,
                                              CDWShrdPtr& object_ptr_ref) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj q = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                                chaos::DataPackCommonKey::DPCK_TIMESTAMP << mongo::Date_t(TimingUtil::getTimeStamp()));
        DEBUG_CODE(DBG<<log_message("getObject",
                                    "findOne",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            q.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                                      q))){
            ERR << "Error fetching stored object";
        } else if(result.isEmpty()) {
            DBG << "No data has been found";
        } else {
            if(result.hasField(MONGODB_DAQ_DATA_FIELD)) {
                mongo::BSONElement e = result.getField(MONGODB_DAQ_DATA_FIELD);
                if(e.isABSONObj()) {
                    object_ptr_ref.reset(new CDataWrapper(e.Obj().objdata()));
                }
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBObjectStorageDataAccess::getLastObject(const std::string& key,
                                                  CDWShrdPtr& object_ptr_ref) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::Query q = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key);
        q = q.sort(BSON(chaos::DataPackCommonKey::DPCK_TIMESTAMP << -1));
        
        DEBUG_CODE(DBG<<log_message("getLastObject",
                                    "findOne",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            q.toString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                                      q))){
            ERR << "Error fetching stored object";
        } else if(result.isEmpty()) {
            DBG << "No data has been found";
        } else {
            if(result.hasField(MONGODB_DAQ_DATA_FIELD)) {
                mongo::BSONElement e = result.getField(MONGODB_DAQ_DATA_FIELD);
                if(e.isABSONObj()) {
                    object_ptr_ref.reset(new CDataWrapper(e.Obj().objdata()));
                }
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = e.getCode();
    }
    return err;
}

void MongoDBObjectStorageDataAccess::addTimeRange(mongo::BSONObjBuilder& builder,
                                                  const std::string& time_operator,
                                                  uint64_t timestamp) {
    if(timestamp == 0) return;
    builder << time_operator << mongo::Date_t(timestamp);
}

int MongoDBObjectStorageDataAccess::deleteObject(const std::string& key,
                                                 uint64_t start_timestamp,
                                                 uint64_t end_timestamp) {
    int err = 0;
    mongo::BSONObjBuilder builder;
    mongo::BSONObjBuilder time_query_builder;
    try {
        builder << chaos::DataPackCommonKey::DPCK_DEVICE_ID << key;
        addTimeRange(time_query_builder, "$gte", start_timestamp);
        addTimeRange(time_query_builder, "$lte", end_timestamp);
        if(start_timestamp ||
           end_timestamp) {
            builder << chaos::DataPackCommonKey::DPCK_TIMESTAMP << time_query_builder.obj();
        }
        
        mongo::BSONObj q = builder.obj();
        
        DEBUG_CODE(DBG<<log_message("deleteObject",
                                    "delete",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            q.jsonString()));)
        //remove in unacknowledge way if something goes wrong successive query will delete it
        if((err = connection->remove(MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                                     q,
                                     false,
                                     &mongo::WriteConcern::unacknowledged))){
            ERR << CHAOS_FORMAT("Error erasing stored object data for key %1% from %2% to %3%", %key%start_timestamp%end_timestamp);
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBObjectStorageDataAccess::findObject(const std::string& key,
                                               uint64_t timestamp_from,
                                               uint64_t timestamp_to,
                                               const uint32_t page_len,
                                               object_storage::abstraction::VectorObject& found_object_page,
                                               SearchSequence& last_record_found_seq) {
    int err = 0;
    std::vector<mongo::BSONObj> object_found;
    try {
        mongo::Query q;
        bool reverse_order = false;
        const std::string run_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::ControlUnitDatapackCommonKey::RUN_ID);
        const std::string counter_key = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID);
        //we have the intervall
        reverse_order = timestamp_from>timestamp_to;
        
        if(reverse_order == false) {
            q = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                     chaos::DataPackCommonKey::DPCK_TIMESTAMP << BSON("$gte" << mongo::Date_t(timestamp_from) <<
                                                                      "$lte" << mongo::Date_t(timestamp_to)) <<
                     run_key << BSON("$gte" << (long long)last_record_found_seq.run_id) <<
                     counter_key << BSON("$gte" << (long long)last_record_found_seq.datapack_counter));
        } else {
            BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                 chaos::DataPackCommonKey::DPCK_TIMESTAMP << BSON("$lte" << mongo::Date_t(timestamp_from) <<
                                                                  "$gte" << mongo::Date_t(timestamp_to)) <<
                 run_key << BSON("$lte" << (long long)last_record_found_seq.run_id) <<
                 counter_key << BSON("$lte" << (long long)last_record_found_seq.datapack_counter));
        }
        
        if(reverse_order) {
            q = q.sort(BSON(run_key<<-1<<counter_key<<-1<<chaos::DataPackCommonKey::DPCK_TIMESTAMP<<-1));
        } else {
            q = q.sort(BSON(run_key<<1<<counter_key<<1<<chaos::DataPackCommonKey::DPCK_TIMESTAMP<<1));
        }
        
        DEBUG_CODE(DBG<<log_message("findObject",
                                    "findN",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            q.toString()));)
        mongo::BSONArrayBuilder bab;
        connection->findN(object_found,
                          MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                          q.readPref(mongo::ReadPreference_SecondaryPreferred, bab.arr()),
                          page_len);
        if(object_found.size() == 0) {
            DBG << "No data has been found";
        } else {
            if(object_found.size()) {
                for(std::vector<mongo::BSONObj>::iterator it = object_found.begin(),
                    end = object_found.end();
                    it != end;
                    it++) {
                    CDWShrdPtr new_obj(new CDataWrapper(it->getObjectField(MONGODB_DAQ_DATA_FIELD).objdata()));
                    found_object_page.push_back(new_obj);
                }
                if( object_found[object_found.size()-1].getFieldDotted(run_key).type()==mongo::NumberInt){
                    last_record_found_seq.run_id = object_found[object_found.size()-1].getFieldDotted(run_key).Number();
                    
                } else {
                    last_record_found_seq.run_id = object_found[object_found.size()-1].getFieldDotted(run_key).Long();
                }
                
                
                if( object_found[object_found.size()-1].getFieldDotted(counter_key).type()==mongo::NumberInt){
                    last_record_found_seq.datapack_counter = object_found[object_found.size()-1].getFieldDotted(counter_key).Number();
                    
                } else {
                    last_record_found_seq.datapack_counter = object_found[object_found.size()-1].getFieldDotted(counter_key).Long();
                }
                DEBUG_CODE(DBG<<CHAOS_FORMAT("Found %1% element last sequence read is [%2%-%3%]", %object_found.size()%(int64_t)last_record_found_seq.run_id%last_record_found_seq.datapack_counter);)
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = e.getCode();
    }
    return err;
}

int MongoDBObjectStorageDataAccess::countObject(const std::string& key,
                                                const uint64_t timestamp_from,
                                                const uint64_t timestamp_to,
                                                const uint64_t& object_count) {
    int err = 0;
    
    return err;
}

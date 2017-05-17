/*
 *	MongoDBObjectStorageDataAccess.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 02/09/16 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#include "MongoDBObjectStorageDataAccess.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::data_service::object_storage::mongodb;


#define INFO INFO_LOG(MongoDBObjectStorageDataAccess)
#define DBG  DBG_LOG(MongoDBObjectStorageDataAccess)
#define ERR  ERR_LOG(MongoDBObjectStorageDataAccess)

#define MONGODB_DAQ_COLL_NAME       "daq"
#define MONGODB_DAQ_DATA_FIELD      "data"

using namespace boost;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::batch_command;
using namespace chaos::service_common::persistence::mongodb;
using namespace chaos::data_service::object_storage::mongodb;
using namespace chaos::data_service::object_storage::abstraction;

MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(const ChaosSharedPtr<service_common::persistence::mongodb::MongoDBHAConnectionManager>& _connection):
MongoDBAccessor(_connection){}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {}

int MongoDBObjectStorageDataAccess::pushObject(const std::string& key,
                                               const CDataWrapper& stored_object) {
    int err = 0;
    int bson_raw_data_size = 0;
    const char *bson_raw_data = NULL;
    try {
        
        bson_raw_data = stored_object.getBSONRawData(bson_raw_data_size);
        
        mongo::BSONObj q = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                                chaos::DataPackCommonKey::DPCK_TIMESTAMP << mongo::Date_t(TimingUtil::getTimeStamp()) <<
                                MONGODB_DAQ_DATA_FIELD << mongo::BSONObj(bson_raw_data));
        if((err = connection->insert(MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                                     q,
                                     mongo::WriteConcern::unacknowledged))){
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
                                              ObjectSharedPtr& object_ptr_ref) {
    int err = 0;
    mongo::BSONObj result;
    try {
        mongo::BSONObj q = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                                chaos::DataPackCommonKey::DPCK_TIMESTAMP << mongo::Date_t(TimingUtil::getTimeStamp()));
        
        mongo::BSONObj p = BSON(CHAOS_FORMAT("%1%.$",%MONGODB_DAQ_DATA_FIELD) << 1);
        DEBUG_CODE(DBG<<log_message("getObject",
                                    "findOne",
                                    DATA_ACCESS_LOG_2_ENTRY("Query",
                                                            "Projection",
                                                            q.jsonString(),
                                                            p.jsonString()));)
        
        if((err = connection->findOne(result,
                                      MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                                      q,
                                      &p))){
            ERR << "Error fetching stored object";
        } else if(result.isEmpty()) {
            DBG << "No data has been found";
        } else {
            object_ptr_ref.reset(new CDataWrapper(result.objdata()));
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
                                     mongo::WriteConcern::unacknowledged))){
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
                                               uint64_t& last_sequence_found) {
    int err = 0;
    std::vector<mongo::BSONObj> object_found;
    try {
        bool reverse_order = false;
        const std::string sequence_field = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_SEQ_ID);
        mongo::BSONObjBuilder time_query;
            //we have the intervall
            reverse_order = timestamp_from>timestamp_to;
#ifndef QUERY_WITH_SEQUENCE
            if(last_sequence_found>0){
            	timestamp_from = last_sequence_found;
            }
#endif

            if(reverse_order == false) {
                time_query << "$gt" << mongo::Date_t(timestamp_from) <<
                               "$lte" << mongo::Date_t(timestamp_to);
            } else {
                time_query << "$lt" << mongo::Date_t(timestamp_from) <<
                              "$gte" << mongo::Date_t(timestamp_to);
            }


#ifdef QUERY_WITH_SEQUENCE


        mongo::BSONObj o = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                                sequence_field << BSON("$gt" << (long long)last_sequence_found) <<
                                chaos::DataPackCommonKey::DPCK_TIMESTAMP << time_query.obj());
#else
        mongo::BSONObj o = BSON(chaos::DataPackCommonKey::DPCK_DEVICE_ID << key <<
                                   chaos::DataPackCommonKey::DPCK_TIMESTAMP << time_query.obj());
#endif
        mongo::Query q = o;
        
        if(reverse_order) {
            q = q.sort(BSON(chaos::DataPackCommonKey::DPCK_TIMESTAMP<<-1));
        } else {
            q = q.sort(BSON(chaos::DataPackCommonKey::DPCK_TIMESTAMP<<1));
        }
        
        DEBUG_CODE(DBG<<log_message("findObject",
                                    "findN",
                                    DATA_ACCESS_LOG_1_ENTRY("Query",
                                                            o.jsonString()));)
        
        connection->findN(object_found,
                          MONGO_DB_COLLECTION_NAME(MONGODB_DAQ_COLL_NAME),
                          q,
                          page_len);
        if(object_found.size() == 0) {
            DBG << "No data has been found";
        } else {
            if(object_found.size()) {
                for(std::vector<mongo::BSONObj>::iterator it = object_found.begin(),
                    end = object_found.end();
                    it != end;
                    it++) {
                    found_object_page.push_back(ObjectSharedPtr(new CDataWrapper(it->getObjectField(MONGODB_DAQ_DATA_FIELD).objdata())));
                }
#ifdef QUERY_WITH_SEQUENCE
                last_sequence_found = object_found[object_found.size()-1].getFieldDotted(sequence_field).Long();
                DEBUG_CODE(DBG<<CHAOS_FORMAT("Found %1% element last sequence read is %2%", %object_found.size()%(int64_t)last_sequence_found);)
#else
                const std::string sequence_time_field = CHAOS_FORMAT("%1%.%2%",%MONGODB_DAQ_DATA_FIELD%chaos::DataPackCommonKey::DPCK_TIMESTAMP );

                last_sequence_found = object_found[object_found.size()-1].getFieldDotted(sequence_time_field).Long();
                DEBUG_CODE(DBG<<CHAOS_FORMAT("Found %1% element last TIMED sequence read is %2%", %object_found.size()%(int64_t)last_sequence_found);)
#endif
            }
        }
    } catch (const mongo::DBException &e) {
        ERR << e.what();
        err = e.getCode();
    }
    return err;
}

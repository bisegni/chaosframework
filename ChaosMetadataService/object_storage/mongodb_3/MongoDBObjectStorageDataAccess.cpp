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

  try {
    //get client the connection
    auto client = pool_ref.acquire();

    //access to database
    auto db = (*client)[MONGODB_DB_NAME];
    //access a collection
    mongocxx::collection coll = db[MONGODB_DAQ_COLL_NAME];

    int buffer_size = 0;

    const int64_t now_in_ms = TimingUtil::getTimeStamp() & 0xFFFFFFFFFFFFFF00;

    auto now_in_ms_bson = bsoncxx::types::b_date(std::chrono::milliseconds(now_in_ms));

    const char* buffer = stored_object.getBSONRawData(buffer_size);

    // basic::document builds a BSON document.
    auto builder = builder::basic::document{};
    builder.append(bsoncxx::builder::basic::kvp(std::string(chaos::DataPackCommonKey::DPCK_DEVICE_ID), key));
    builder.append(bsoncxx::builder::basic::kvp(std::string(chaos::DataPackCommonKey::DPCK_TIMESTAMP), now_in_ms_bson));

    bsoncxx::builder::basic::document zone_pack = shrd_key_manager.getNewDataPack(key, now_in_ms, buffer_size);
    builder.append(bsoncxx::builder::concatenate(zone_pack.view()));

    bsoncxx::document::view view_daq_data((const std::uint8_t*)stored_object.getBSONRawData(), stored_object.getBSONRawSize());
    builder.append(bsoncxx::builder::basic::kvp(MONGODB_DAQ_DATA_FIELD, view_daq_data));

    bsoncxx::document::view view = builder.view();
    try {
      coll.insert_one(builder.view());

    } catch (const mongocxx::bulk_write_exception& e) {
      // We can compare the error_code to a known server side error code number
      if (e.code().value() != 11000) {
        return EXIT_FAILURE;
      }

      std::cout << e.what() << std::endl;
      if (e.raw_server_error()) {
        std::cout << "Raw server error:" << std::endl;
        std::cout << bsoncxx::to_json(*(e.raw_server_error())) << std::endl;
      }
      std::cout << std::endl;
    }

  } catch (const mongocxx::logic_error& e) {
    ERR << e.what();
    err = e.code().value();
  }

  return err;
}

int MongoDBObjectStorageDataAccess::getObject(const std::string& key,
                                              const uint64_t& timestamp,
                                              CDWShrdPtr& object_ptr_ref) {
    int err = 0;
    mongo::BSONObj result;
    return err;
}

int MongoDBObjectStorageDataAccess::getLastObject(const std::string& key,
                                                  CDWShrdPtr& object_ptr_ref) {
    int err = 0;

    return err;
}

//void MongoDBObjectStorageDataAccess::addTimeRange(mongo::BSONObjBuilder& builder,
//                                                  const std::string& time_operator,
//                                                  uint64_t timestamp) {
//    if(timestamp == 0) return;
//    builder << time_operator << mongo::Date_t(timestamp);
//}

int MongoDBObjectStorageDataAccess::deleteObject(const std::string& key,
                                                 uint64_t start_timestamp,
                                                 uint64_t end_timestamp) {
    int err = 0;
    auto client  = pool_ref.acquire();

    return err;
}

int MongoDBObjectStorageDataAccess::findObject(const std::string&                                          key,
                                               const ChaosStringSet&                                       meta_tags,
                                               const uint64_t                                              timestamp_from,
                                               const uint64_t                                              timestamp_to,
                                               const uint32_t                                              page_len,
                                               abstraction::VectorObject&                                  found_object_page,
                                               common::direct_io::channel::opcode_headers::SearchSequence& last_record_found_seq) {
  int                         err = 0;
  std::vector<mongo::BSONObj> object_found;
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

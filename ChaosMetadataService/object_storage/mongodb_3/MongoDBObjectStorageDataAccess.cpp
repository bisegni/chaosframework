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
#include "MongoDBObjectStorageDataAccess.h"

#include <chaos/common/global.h>

#include "MongoDBObjectStorageDataAccess.h"
#include "../../ChaosMetadataService.h"

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>

//using namespace chaos::data_service::object_storage::mongodb;


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
using namespace chaos::data_service::object_storage::mongodb_3;
using namespace chaos::data_service::object_storage::abstraction;

MongoDBObjectStorageDataAccess::MongoDBObjectStorageDataAccess(mongocxx::pool& _pool_ref):
pool_ref(_pool_ref){}

MongoDBObjectStorageDataAccess::~MongoDBObjectStorageDataAccess() {}

int MongoDBObjectStorageDataAccess::pushObject(const std::string& key,
                                               const CDataWrapper& stored_object) {
    int err = 0;
    return err;
}

int MongoDBObjectStorageDataAccess::getObject(const std::string& key,
                                              const uint64_t& timestamp,
                                              CDWShrdPtr& object_ptr_ref) {
    int err = 0;
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
    return err;
}

int MongoDBObjectStorageDataAccess::findObject(const std::string& key,
                                               uint64_t timestamp_from,
                                               uint64_t timestamp_to,
                                               const uint32_t page_len,
                                               object_storage::abstraction::VectorObject& found_object_page,
                                               SearchSequence& last_record_found_seq) {
    int err = 0;
    return err;
}

int MongoDBObjectStorageDataAccess::countObject(const std::string& key,
                                                const uint64_t timestamp_from,
                                                const uint64_t timestamp_to,
                                                const uint64_t& object_count) {
    int err = 0;
    return err;
}

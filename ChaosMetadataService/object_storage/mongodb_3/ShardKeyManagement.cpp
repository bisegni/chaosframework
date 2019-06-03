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

#include <chaos/common/global.h>
#include <ChaosMetadataService/object_storage/mongodb_3/ShardKeyManagement.h>

#include <limits>

using namespace chaos::metadata_service::object_storage::mongodb_3;

using namespace bsoncxx;
using namespace bsoncxx::builder::basic;

#define INFO INFO_LOG(ShardKeyManagement)
#define DBG  DBG_LOG(ShardKeyManagement)
#define ERR  ERR_LOG(ShardKeyManagement)

#pragma mark DaqZoneInfo
DaqZonedInfo::DaqZonedInfo():
index_zone_doc(builder::basic::document{}),
data_zone_doc(builder::basic::document{}){}

builder::basic::document& DaqZonedInfo::getIndexDocument() {
    return index_zone_doc;
}

builder::basic::document& DaqZonedInfo::getDataDocument() {
    return data_zone_doc;
}

#pragma mark KeyRNDShardInfo

#define CHECK_TIMEOUT_IN_MS         30000         //(30 seconds)
#define DEFAULT_KEY_STORAGE_QUOTA   10485760      //size in kbyle that when it i exceeded a new key is calculated
#define DEFAULT_KEY_TIMEOUT         1000*60*60    //time in millisconds that whene it is exceeded a new key is calculated

boost::random_device KeyRNDShardInfo::rd;

KeyRNDShardInfo::KeyRNDShardInfo(const std::string& _key,
                                 const uint32_t _storage_quota):
rnd_gen_int64(),
key(_key),
storage_quota(_storage_quota),
next_check_ts(0),
next_timeout_ts(0),
stored_byte(0),
shard_value(rnd_gen_int64(rd)){}


KeyRNDShardInfo::~KeyRNDShardInfo(){}

const int64_t KeyRNDShardInfo::getShardValue(const int64_t now_in_mds,
                                             const uint32_t new_size_byte) {
    ChaosWriteLock wl(lock_mutex);
    stored_byte += new_size_byte;
    if(now_in_mds > next_check_ts) {
        next_check_ts = now_in_mds+CHECK_TIMEOUT_IN_MS;
        if(stored_byte >= storage_quota ||
           now_in_mds >= next_timeout_ts) {
            next_timeout_ts = now_in_mds + DEFAULT_KEY_TIMEOUT;
            stored_byte = 0;
            //generate new value
            shard_value = rnd_gen_int64(rd);
            DEBUG_CODE(DBG << CHAOS_FORMAT("Generate new random shard key for %1%: -> %2%", %key%shard_value););
        }
    }
    return shard_value;
}

#pragma mark ShardKeyManagement

ShardKeyManagement::ShardKeyManagement():
zone_alias("default"){}

ShardKeyManagement::~ShardKeyManagement(){}

void ShardKeyManagement::setZoneAlias(const std::string& new_zone_alias) {
    zone_alias = new_zone_alias;
}

DaqZonedInfo ShardKeyManagement::getNewDataPack(const std::string& key,
                                                                const int64_t now_in_ms,
                                                                const uint32_t new_size_byte) {
    DaqZonedInfo zone_info;
    ChaosSharedPtr<KeyRNDShardInfo> shard_info_ptr;
    MapKeyShardInfo_LWriteLock wr = map_key_shard_info.getWriteLockObject();
    MapKeyShardInfoIterator shard_info_it = map_key_shard_info().find(key);
    if(shard_info_it == map_key_shard_info().end()) {
        std::pair <MapKeyShardInfoIterator, bool> insert_result;
        insert_result = map_key_shard_info().insert(MapKeyShardInfoPair(key, ChaosMakeSharedPtr<KeyRNDShardInfo>(key, DEFAULT_KEY_STORAGE_QUOTA)));
        assert(insert_result.second);
        shard_info_it = insert_result.first;
    }
    wr->unlock();

    int64_t shard_info = shard_info_it->second->getShardValue(now_in_ms, new_size_byte);
    zone_info.getIndexDocument().append(bsoncxx::builder::basic::kvp("zone_key", zone_alias+"_index"));
    zone_info.getIndexDocument().append(bsoncxx::builder::basic::kvp("shard_key", shard_info));

    zone_info.getDataDocument().append(bsoncxx::builder::basic::kvp("zone_key", zone_alias+"_data"));
    zone_info.getDataDocument().append(bsoncxx::builder::basic::kvp("shard_key", shard_info));
    return zone_info;
}

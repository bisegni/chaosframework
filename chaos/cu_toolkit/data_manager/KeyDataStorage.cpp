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
#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>
using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::data::structured;
using namespace chaos::common::utility;
using namespace chaos::cu::data_manager;

#define KeyDataStorageLAPP	INFO_LOG(KeyDataStorage)
#define KeyDataStorageLDBG	DBG_LOG(KeyDataStorage)
#define KeyDataStorageLERR	ERR_LOG(KeyDataStorage)

#pragma mark KeyDataStorage
KeyDataStorage::KeyDataStorage(const std::string& _key,
                               chaos_io::IODataDriver *_io_data_driver):
key(_key),
io_data_driver(_io_data_driver),
storage_type(DataServiceNodeDefinitionType::DSStorageTypeLive),
storage_history_time(0),
storage_history_time_last_push(0),
storage_live_time(0),
storage_live_time_last_push(0),
use_timing_info(true),
output_key(key + DataPackPrefixID::OUTPUT_DATASET_POSTFIX),
input_key(key + DataPackPrefixID::INPUT_DATASET_POSTFIX),
system_key(key + DataPackPrefixID::SYSTEM_DATASET_POSTFIX),
custom_key(key + DataPackPrefixID::CUSTOM_DATASET_POSTFIX),
health_key(key + DataPackPrefixID::HEALTH_DATASET_POSTFIX),
cu_alarm_key(key + DataPackPrefixID::CU_ALARM_DATASET_POSTFIX),
dev_alarm_key(key + DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX){
    //set sequence id for all domain
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainOutput, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainInput, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainCustom, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainSystem, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainHealth, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainDevAlarm, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));
    map_seq_id.insert(DSSeqIDMapPair(KeyDataStorageDomainCUAlarm, ChaosSharedPtr< boost::atomic<int64_t> >(new boost::atomic<int64_t>())));

}

KeyDataStorage::~KeyDataStorage() {
    restore_point_map.clear();
}

void KeyDataStorage::init(void *init_parameter)  {
    if(!io_data_driver.get()) throw chaos::CException(-1, "IO data driver not set", __PRETTY_FUNCTION__);
    
    io_data_driver->init(init_parameter);
}

void KeyDataStorage::deinit()  {
    if(io_data_driver.get()) {io_data_driver->deinit();}
    restore_point_map.clear();
}

std::string KeyDataStorage::getDomainString(const KeyDataStorageDomain dataset_domain) {
    switch(dataset_domain) {
        case DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            return output_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            return input_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            return system_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            return custom_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            return cu_alarm_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            return dev_alarm_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH:
            return health_key;
            break;
    }
    return "";
}


std::string KeyDataStorage::getDomainString(const std::string& node_uid,
                                            const KeyDataStorageDomain dataset_domain) {
    switch(dataset_domain) {
        case DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            return node_uid +  DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            return node_uid +  DataPackPrefixID::INPUT_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            return node_uid +  DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            return node_uid +  DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            return node_uid +  DataPackPrefixID::CU_ALARM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            return node_uid +  DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH:
            return node_uid +  DataPackPrefixID::HEALTH_DATASET_POSTFIX;
            break;
    }
    return "";
}

/*
 Return a new instace for the CSDatawrapped filled
 with default madatory data
 */
CDWShrdPtr KeyDataStorage::getNewDataPackForDomain(const KeyDataStorageDomain domain) {
    CDWShrdPtr result = ChaosMakeSharedPtr<CDataWrapper>();
    int32_t node_type;
    switch(domain) {
        case KeyDataStorageDomainOutput:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT;
            break;
        case KeyDataStorageDomainInput:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_INPUT;
            break;
        case KeyDataStorageDomainSystem:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM;
            break;
        case KeyDataStorageDomainCustom:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM;
            break;
        case KeyDataStorageDomainCUAlarm:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM;
            break;
        case KeyDataStorageDomainDevAlarm:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM;
            break;
        case KeyDataStorageDomainHealth:
            node_type = DataPackCommonKey::DPCK_DATASET_TYPE_HEALTH;
            break;
    }
    //add the unique key
    result->addStringValue(DataPackCommonKey::DPCK_DEVICE_ID, key);
    result->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, node_type);
    result->addInt64Value(DataPackCommonKey::DPCK_SEQ_ID, ++(*map_seq_id[domain]));
    return result;
}

int KeyDataStorage::pushDataWithControlOnHistoryTime(const std::string& key,
                                                      CDWShrdPtr dataset,
                                                      DataServiceNodeDefinitionType::DSStorageType storage_type) {
    
    uint64_t now = TimingUtil::getTimeStampInMicroseconds();
    int effective_storage_type = DataServiceNodeDefinitionType::DSStorageTypeUndefined;
    int err=0;
    if(storage_type & DataServiceNodeDefinitionType::DSStorageTypeLive ||
       override_storage_everride & DataServiceNodeDefinitionType::DSStorageTypeLive) {
        //live is enbaled
//        if(use_timing_info) {
        if((now - storage_live_time_last_push) >= storage_live_time) {
            effective_storage_type |= DataServiceNodeDefinitionType::DSStorageTypeLive;
            storage_live_time_last_push = now;
        }
//        } else {
//            effective_storage_type |= DataServiceNodeDefinitionType::DSStorageTypeLive;
//        }
    }
    
    if(storage_type & DataServiceNodeDefinitionType::DSStorageTypeHistory ||
       override_storage_everride & DataServiceNodeDefinitionType::DSStorageTypeHistory) {
        //history is enabled
        if(use_timing_info) {
            if((now - storage_history_time_last_push) >= storage_history_time) {
                effective_storage_type |= DataServiceNodeDefinitionType::DSStorageTypeHistory;
                storage_history_time_last_push = now;
            }
        } else {
            effective_storage_type |= DataServiceNodeDefinitionType::DSStorageTypeHistory;
        }
    }
    
    if(effective_storage_type) {
        err=io_data_driver->storeData(key,
                                  MOVE(dataset),
                                  static_cast<DataServiceNodeDefinitionType::DSStorageType>(effective_storage_type),
                                  current_tags());
    }
    return err;
}

int KeyDataStorage::pushDataSet(KeyDataStorageDomain domain,
                                 CDWShrdPtr dataset) {
    CHAOS_ASSERT(io_data_driver.get());
    LChaosStringSetReadLock wl = current_tags.getReadLockObject();
    int err=0;
    //lock for protect the access
    boost::unique_lock<boost::mutex> l(mutex_push_data);
    switch(domain) {
        case KeyDataStorageDomainOutput:
            err=pushDataWithControlOnHistoryTime(output_key,
                                             MOVE(dataset),
                                             storage_type);
            break;
        case KeyDataStorageDomainInput:
            //input channel need to be push ever either in live and in history
            err=io_data_driver->storeData(input_key,
                                      MOVE(dataset),
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory,
                                      current_tags());
            break;
        case KeyDataStorageDomainSystem:
            //system channel need to be push ever either in live and in history
            err=io_data_driver->storeData(system_key,
                                      MOVE(dataset),
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory,
                                      current_tags());
            break;
        case KeyDataStorageDomainCUAlarm:
            //system channel need to be push ever either in live and in history
            err=io_data_driver->storeData(cu_alarm_key,
                                      MOVE(dataset),
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory,
                                      current_tags());
            break;
        case KeyDataStorageDomainDevAlarm:
            //system channel need to be push ever either in live and in history
            err=io_data_driver->storeData(dev_alarm_key,
                                      MOVE(dataset),
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory,
                                      current_tags());
            break;
        case KeyDataStorageDomainHealth:
            //system channel need to be push ever either in live and in history
            err=io_data_driver->storeHealthData(health_key,
                                            MOVE(dataset),
                                            DataServiceNodeDefinitionType::DSStorageTypeLiveHistory,
                                            current_tags());
            break;
        case KeyDataStorageDomainCustom:
            err=pushDataWithControlOnHistoryTime(custom_key,
                                             MOVE(dataset),
                                             storage_type);
            break;
    }
    return err;
}

int KeyDataStorage::loadRestorePoint(const std::string& restore_point_tag) {
    int err = 0;
    CDWShrdPtr dataset;
    
    if(!restore_point_map.count(restore_point_tag)) {
        //allocate map for the restore tag
        restore_point_map.insert(make_pair(restore_point_tag, std::map<std::string, ChaosSharedPtr<chaos_data::CDataWrapper> >()));
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainOutput,
                                                             dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainOutput from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(output_key, MOVE(dataset)));dataset.reset();
        }
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainInput,
                                                             dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainInput from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(input_key, MOVE(dataset)));dataset.reset();
            
        }
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainCustom,
                                                             dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainCustom from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(custom_key, MOVE(dataset)));dataset.reset();
        }
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainSystem,
                                                             dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainSystem from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(system_key, MOVE(dataset)));dataset.reset();
        }
    }
    
    return err;
}

int KeyDataStorage::clearRestorePoint(const std::string& restore_point_tag) {
    int err = 0;
    if(restore_point_map.count(restore_point_tag)) {
        restore_point_map[restore_point_tag].clear();
    }
    return err;
    
}

ChaosSharedPtr<chaos_data::CDataWrapper> KeyDataStorage::getDatasetFromRestorePoint(const std::string& restore_point_tag,
                                                                                    KeyDataStorageDomain domain) {
    if(!restore_point_map.count(restore_point_tag)) {
        return ChaosSharedPtr<chaos_data::CDataWrapper>();
    }
    
    switch(domain) {
        case KeyDataStorageDomainOutput:
            return restore_point_map[restore_point_tag][output_key];
        case KeyDataStorageDomainInput:
            return restore_point_map[restore_point_tag][input_key];
        case KeyDataStorageDomainCustom:
            return restore_point_map[restore_point_tag][custom_key];
        case KeyDataStorageDomainSystem:
            return restore_point_map[restore_point_tag][system_key];
        default:
            CHAOS_ASSERT(false)
            break;
    }
}

void KeyDataStorage::updateConfiguration(CDataWrapper *configuration) {
    //update the driver configration
    if(io_data_driver.get()) io_data_driver->updateConfiguration(configuration);
    
}

void KeyDataStorage::updateConfiguration(const std::string& conf_name,
                                         const chaos::common::data::CDataVariant& conf_value) {
    if(conf_name.compare(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME) == 0){
        storage_history_time = conf_value.asUInt64();
    } else if(conf_name.compare(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME) == 0){
        storage_live_time = conf_value.asUInt64();
    } else if(conf_name.compare(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE) == 0){
        storage_type = static_cast<DataServiceNodeDefinitionType::DSStorageType>(conf_value.asInt32());
    }
    KeyDataStorageLAPP << CHAOS_FORMAT("Set value %1% to property %2%", %conf_value.asString()%conf_name);
}

void KeyDataStorage::setOverrideStorageType(chaos::DataServiceNodeDefinitionType::DSStorageType _override_storage_type) {
    override_storage_everride = _override_storage_type;
}

void KeyDataStorage::setTimingConfigurationBehaviour(bool _use_timing_info) {
    use_timing_info = _use_timing_info;
}

void KeyDataStorage::addTag(const std::string& tag) {
    LChaosStringSetWriteLock wl = current_tags.getWriteLockObject();
    current_tags().insert(tag);
}

void KeyDataStorage::addTag(const ChaosStringSet& tags) {
    LChaosStringSetWriteLock wl = current_tags.getWriteLockObject();
    current_tags().insert(tags.begin(), tags.end());
}

void KeyDataStorage::removeTag(const std::string& tag) {
    LChaosStringSetWriteLock wl = current_tags.getWriteLockObject();
    current_tags().erase(tag);
}

void KeyDataStorage::removeTag(const ChaosStringSet& tags) {
    LChaosStringSetWriteLock wl = current_tags.getWriteLockObject();
    current_tags().erase(tags.begin(), tags.end());
}

DataServiceNodeDefinitionType::DSStorageType KeyDataStorage::getStorageType() {
    return storage_type;
}

uint64_t KeyDataStorage::getStorageLiveTime() {
    return storage_live_time;
}

uint64_t KeyDataStorage::getStorageHistoryTime() {
    return storage_history_time;
}

int KeyDataStorage::performLiveFetch(const KeyDataStorageDomain dataset_domain,
                                     chaos::common::data::CDWShrdPtr& found_dataset) {
    int err = 0;
    size_t size;
    std::string node_dataset = getDomainString(dataset_domain);
    char * raw_data = io_data_driver->retriveRawData(node_dataset, &size);
    if(raw_data) {
        found_dataset.reset(new CDataWrapper(raw_data));
    }
    delete[](raw_data);
    return err;
}

int KeyDataStorage::performLiveFetch(const ChaosStringVector& node_uid,
                                     const KeyDataStorageDomain dataset_domain,
                                     chaos::common::data::VectorCDWShrdPtr& found_dataset) {
    ChaosStringVector real_node_uid;
    for(ChaosStringVectorConstIterator it = node_uid.begin(),
        end = node_uid.end();
        it != end;
        it++) {
        real_node_uid.push_back(getDomainString(*it, dataset_domain));
    }
    return io_data_driver->retriveMultipleData(real_node_uid,
                                               found_dataset);
}

int KeyDataStorage::performSelfQuery(chaos::common::io::QueryCursor **cursor,
                                     const KeyDataStorageDomain dataset_domain,
                                     const uint64_t start_ts,
                                     const uint64_t end_ts,
                                     const ChaosStringSet& meta_tags) {
    *cursor = io_data_driver->performQuery(getDomainString(dataset_domain),
                                           start_ts,
                                           end_ts,
                                           meta_tags);
    return ((*cursor == NULL)?-1:0);
}

int KeyDataStorage::performGeneralQuery(chaos::common::io::QueryCursor **cursor,
                                        const std::string& node_id,
                                        KeyDataStorageDomain dataset_domain,
                                        const uint64_t start_ts,
                                        const uint64_t end_ts,
                                        const ChaosStringSet& meta_tags) {
    *cursor = io_data_driver->performQuery(getDomainString(node_id,
                                                           dataset_domain),
                                           start_ts,
                                           end_ts,
                                           meta_tags);
    return ((*cursor == NULL)?-1:0);
}

void KeyDataStorage::releseQuery(chaos::common::io::QueryCursor *cursor) {
    io_data_driver->releaseQuery(cursor);
}

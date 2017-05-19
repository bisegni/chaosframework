/*
 *	KeyDataStorage.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/cu_toolkit/data_manager/KeyDataStorage.h>
using namespace std;
using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::cu::data_manager;

#define KeyDataStorageLAPP	INFO_LOG(KeyDataStorage)
#define KeyDataStorageLDBG	DBG_LOG(KeyDataStorage)
#define KeyDataStorageLERR	ERR_LOG(KeyDataStorage)


KeyDataStorage::KeyDataStorage(const std::string& _key,
                               chaos_io::IODataDriver *_io_data_driver):
key(_key),
io_data_driver(_io_data_driver),
storage_type(DataServiceNodeDefinitionType::DSStorageTypeLive),
storage_history_time(0),
storage_history_time_last_push(0),
storage_live_time(0),
storage_live_time_last_push(0),
sequence_id(0 /*std::numeric_limits<int64_t>::min()*/){
    output_key	= _key + DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
    input_key	= _key + DataPackPrefixID::INPUT_DATASET_POSTFIX;
    system_key	= _key + DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
    custom_key	= _key + DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
    cu_alarm_key	= _key + DataPackPrefixID::CU_ALARM_DATASET_POSTFIX;
    dev_alarm_key	= _key + DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX;
}

KeyDataStorage::~KeyDataStorage() {
    restore_point_map.clear();
    if(io_data_driver) {
        delete(io_data_driver);
        io_data_driver = NULL;
    }
    
}

void KeyDataStorage::init(void *init_parameter) throw (chaos::CException) {
    if(!io_data_driver) throw chaos::CException(-1, "IO data driver not set", __PRETTY_FUNCTION__);
    
    io_data_driver->init(init_parameter);
}

void KeyDataStorage::deinit() throw (chaos::CException) {
    if(io_data_driver) io_data_driver->deinit();
    restore_point_map.clear();
}
/*
 Return a new instace for the CSDatawrapped filled
 with default madatory data
 */
CDataWrapper* KeyDataStorage::getNewDataPackForDomain(const KeyDataStorageDomain domain) {
    CDataWrapper *result = new CDataWrapper();
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
    }
    //add the unique key
    result->addStringValue(DataPackCommonKey::DPCK_DEVICE_ID, key);
    result->addInt32Value(DataPackCommonKey::DPCK_DATASET_TYPE, node_type);
    result->addInt64Value(DataPackCommonKey::DPCK_SEQ_ID, ++sequence_id);
    return result;
}

/*
 Retrive the data from Live Storage
 */
ArrayPointer<CDataWrapper>* KeyDataStorage::getLastDataSet(KeyDataStorageDomain domain) {
    //retrive data from cache for the key managed by
    //this instance of keydatastorage
    CHAOS_ASSERT(io_data_driver);
    //lock for protect the access
    boost::unique_lock<boost::mutex> l(mutex_push_data);
    switch(domain) {
        case KeyDataStorageDomainOutput:
            return io_data_driver->retriveData(output_key);
            break;
        case KeyDataStorageDomainInput:
            return io_data_driver->retriveData(input_key);
            break;
        case KeyDataStorageDomainSystem:
            return io_data_driver->retriveData(system_key);
            break;
        case KeyDataStorageDomainCustom:
            return io_data_driver->retriveData(custom_key);
            break;
        case KeyDataStorageDomainCUAlarm:
            return io_data_driver->retriveData(cu_alarm_key);
            break;
        case KeyDataStorageDomainDevAlarm:
            return io_data_driver->retriveData(dev_alarm_key);
            break;
    }
    
}

void KeyDataStorage::pushDataWithControlOnHistoryTime(const std::string& key,
                                                      CDataWrapper *dataToStore,
                                                      DataServiceNodeDefinitionType::DSStorageType storage_type) {
    uint64_t now = TimingUtil::getTimeStampInMicroseconds();
    switch(storage_type) {
        case DataServiceNodeDefinitionType::DSStorageTypeLive: {
            if((now - storage_live_time_last_push) >= storage_live_time) {
                io_data_driver->storeData(key,
                                          dataToStore,
                                          DataServiceNodeDefinitionType::DSStorageTypeLive);
                storage_live_time_last_push = now;
            }
            
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeHistory: {
            
            if((now - storage_history_time_last_push) >= storage_history_time) {
                io_data_driver->storeData(key,
                                          dataToStore,
                                          DataServiceNodeDefinitionType::DSStorageTypeHistory);
                storage_history_time_last_push = now;
            }
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeLiveHistory: {
            
            DataServiceNodeDefinitionType::DSStorageType effective_storage_type = DataServiceNodeDefinitionType::DSStorageTypeLiveHistory;
            bool push_history = (now - storage_history_time_last_push) >= storage_history_time;
            bool push_live = (now - storage_live_time_last_push) >= storage_live_time;
            if(push_live || push_history) {
                if(push_history && push_live) {
                    storage_history_time_last_push = storage_live_time_last_push = now;
                } else if(push_history) {
                    effective_storage_type = DataServiceNodeDefinitionType::DSStorageTypeHistory;
                    storage_history_time_last_push = now;
                } else if(push_live) {
                    effective_storage_type = DataServiceNodeDefinitionType::DSStorageTypeLive;
                    storage_live_time_last_push = now;
                }
                io_data_driver->storeData(key,
                                          dataToStore,
                                          effective_storage_type);
            }
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeUndefined:
            break;
    }
}

void KeyDataStorage::pushDataSet(KeyDataStorageDomain domain,
                                 chaos_data::CDataWrapper *dataset) {
    CHAOS_ASSERT(io_data_driver);
    //lock for protect the access
    boost::unique_lock<boost::mutex> l(mutex_push_data);
    switch(domain) {
        case KeyDataStorageDomainOutput:
            pushDataWithControlOnHistoryTime(output_key,
                                             dataset,
                                             storage_type);
            break;
        case KeyDataStorageDomainInput:
            //input channel need to be push ever either in live and in history
            io_data_driver->storeData(input_key,
                                      dataset,
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
            break;
        case KeyDataStorageDomainSystem:
            //system channel need to be push ever either in live and in history
            io_data_driver->storeData(system_key,
                                      dataset,
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
            break;
        case KeyDataStorageDomainCUAlarm:
            //system channel need to be push ever either in live and in history
            io_data_driver->storeData(cu_alarm_key,
                                      dataset,
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
            break;
        case KeyDataStorageDomainDevAlarm:
            //system channel need to be push ever either in live and in history
            io_data_driver->storeData(dev_alarm_key,
                                      dataset,
                                      DataServiceNodeDefinitionType::DSStorageTypeLiveHistory);
            break;
        case KeyDataStorageDomainCustom:
            pushDataWithControlOnHistoryTime(custom_key,
                                             dataset,
                                             storage_type);
            break;
    }
}

int KeyDataStorage::loadRestorePoint(const std::string& restore_point_tag) {
    int err = 0;
    chaos_data::CDataWrapper *dataset = NULL;
    
    if(!restore_point_map.count(restore_point_tag)) {
        //allocate map for the restore tag
        restore_point_map.insert(make_pair(restore_point_tag, std::map<std::string, ChaosSharedPtr<chaos_data::CDataWrapper> >()));
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainOutput,
                                                             &dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainOutput from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(output_key, ChaosSharedPtr<chaos_data::CDataWrapper>(dataset)));
            dataset = NULL;
        }
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainInput,
                                                             &dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainInput from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(input_key, ChaosSharedPtr<chaos_data::CDataWrapper>(dataset)));
            dataset = NULL;
        }
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainCustom,
                                                             &dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainCustom from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(custom_key, ChaosSharedPtr<chaos_data::CDataWrapper>(dataset)));
            dataset = NULL;
        }
    }
    
    if((err = io_data_driver->loadDatasetTypeFromSnapshotTag(restore_point_tag,
                                                             key,
                                                             KeyDataStorageDomainSystem,
                                                             &dataset))) {
        KeyDataStorageLERR << " Error loading dataset of domain KeyDataStorageDomainSystem from restore point:" << restore_point_tag << " for the key:" << key;
        clearRestorePoint(restore_point_tag);
        return err;
    } else {
        if(dataset){
            restore_point_map[restore_point_tag].insert(make_pair(system_key, ChaosSharedPtr<chaos_data::CDataWrapper>(dataset)));
            dataset = NULL;
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

CDataWrapper* KeyDataStorage::updateConfiguration(CDataWrapper *newConfiguration) {
    //update the driver configration
    if(io_data_driver) io_data_driver->updateConfiguration(newConfiguration);
    ChaosUniquePtr<chaos::common::data::CDataWrapper> cu_properties;
    CDataWrapper *cu_property_container = NULL;
    if(newConfiguration->hasKey("property_abstract_control_unit") &&
       newConfiguration->isCDataWrapperValue("property_abstract_control_unit")){
        cu_properties.reset(newConfiguration->getCSDataValue("property_abstract_control_unit"));
        cu_property_container = cu_properties.get();
    } else {
        cu_property_container = newConfiguration;
    }
    
    if(cu_property_container->isInt64Value(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME)){
        storage_history_time = cu_property_container->getUInt64Value(DataServiceNodeDefinitionKey::DS_STORAGE_HISTORY_TIME);
        KeyDataStorageLAPP << CHAOS_FORMAT("Set storage history time to %1%", %storage_history_time);
    }
    
    if(cu_property_container->isInt64Value(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME)){
        storage_live_time = cu_property_container->getUInt64Value(DataServiceNodeDefinitionKey::DS_STORAGE_LIVE_TIME);
        KeyDataStorageLAPP << CHAOS_FORMAT("Set storage live time to %1%", %storage_live_time);
    }
    
    if(cu_property_container->hasKey(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE) &&
       cu_property_container->isInt32Value(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE)){
        storage_type = static_cast<DataServiceNodeDefinitionType::DSStorageType>(cu_property_container->getInt32Value(DataServiceNodeDefinitionKey::DS_STORAGE_TYPE));
        KeyDataStorageLAPP << CHAOS_FORMAT("Set storage type to %1%", %storage_type);
    }
    return NULL;
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

int KeyDataStorage::performSelfQuery(chaos::common::io::QueryCursor **cursor,
                                     KeyDataStorageDomain dataset_domain,
                                     const uint64_t start_ts,
                                     const uint64_t end_ts) {
    std::string node_dataset;
    switch(dataset_domain) {
        case DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            node_dataset = output_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            node_dataset = input_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            node_dataset = system_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            node_dataset = custom_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            node_dataset = cu_alarm_key;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            node_dataset = dev_alarm_key;
            break;
    }
    *cursor = io_data_driver->performQuery(node_dataset,
                                           start_ts,
                                           end_ts);
    return ((*cursor == NULL)?-1:0);
}

int KeyDataStorage::performGeneralQuery(chaos::common::io::QueryCursor **cursor,
                                        const std::string& node_id,
                                        KeyDataStorageDomain dataset_domain,
                                        const uint64_t start_ts,
                                        const uint64_t end_ts) {
    std::string node_dataset;
    switch(dataset_domain) {
        case DataPackCommonKey::DPCK_DATASET_TYPE_OUTPUT:
            node_dataset = node_id + DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_INPUT:
            node_dataset = node_id + DataPackPrefixID::INPUT_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_SYSTEM:
            node_dataset = node_id + DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CUSTOM:
            node_dataset = node_id + DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_CU_ALARM:
            node_dataset = node_id + DataPackPrefixID::CU_ALARM_DATASET_POSTFIX;
            break;
        case DataPackCommonKey::DPCK_DATASET_TYPE_DEV_ALARM:
            node_dataset = node_id + DataPackPrefixID::DEV_ALARM_DATASET_POSTFIX;
            break;
    }
    *cursor = io_data_driver->performQuery(node_dataset,
                                           start_ts,
                                           end_ts);
    return ((*cursor == NULL)?-1:0);
}

void KeyDataStorage::releseQuery(chaos::common::io::QueryCursor *cursor) {
    io_data_driver->releaseQuery(cursor);
}

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
#include "ChaosMetadataService.h"
#include "QueryDataConsumer.h"
#include "DriverPoolManager.h"
#include "worker/DeviceSharedDataWorker.h"
#include "worker/DeviceSharedDataWorkerMetric.h"
#include "worker/DeviceSharedDataWorkerMetricCollector.h"
#include "persistence/persistence.h"

#include <chaos/common/network/NetworkBroker.h>

#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/endianess.h>
#include <chaos/common/utility/DataBuffer.h>
#include <chaos/common/utility/InetUtility.h>

#include <boost/thread.hpp>

using namespace chaos::metadata_service;

using namespace chaos::metadata_service;
using namespace chaos::metadata_service::worker;
using namespace chaos::metadata_service::cache_system;
using namespace chaos::metadata_service::object_storage::abstraction;

using namespace chaos::metadata_service::persistence;
using namespace chaos::metadata_service::persistence::data_access;

using namespace chaos::common::data::structured;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define INFO    INFO_LOG(QueryDataConsumer)
#define DBG     DBG_LOG(QueryDataConsumer)
#define ERR     ERR_LOG(QueryDataConsumer)

ChaosSharedPtr<worker::DeviceSharedDataWorkerMetric> dsdwm_metric;

//constructor
QueryDataConsumer::QueryDataConsumer():
server_endpoint(NULL),
device_channel(NULL),
system_api_channel(NULL),
device_data_worker_index(0){}

QueryDataConsumer::~QueryDataConsumer() {}

void QueryDataConsumer::init(void *init_data)  {
    //get new chaos direct io endpoint
    server_endpoint = NetworkBroker::getInstance()->getDirectIOServerEndpoint();
    if(!server_endpoint) throw chaos::CException(-2, "Invalid server endpoint", __FUNCTION__);
    INFO << "QueryDataConsumer initialized with endpoint "<< server_endpoint->getRouteIndex();
    
    INFO << "Allocating DirectIODeviceServerChannel";
    device_channel = (DirectIODeviceServerChannel*)server_endpoint->getNewChannelInstance("DirectIODeviceServerChannel");
    if(!device_channel) throw chaos::CException(-3, "Error allocating device server channel", __FUNCTION__);
    device_channel->setHandler(this);
    
    INFO << "Allocating DirectIOSystemAPIServerChannel";
    system_api_channel = (DirectIOSystemAPIServerChannel*)server_endpoint->getNewChannelInstance("DirectIOSystemAPIServerChannel");
    if(!system_api_channel) throw chaos::CException(-4, "Error allocating system api server channel", __FUNCTION__);
    system_api_channel->setHandler(this);
    
    //device data worker instances
    for(int idx = 0;
        idx < ChaosMetadataService::getInstance()->setting.worker_setting.instances;
        idx++) {
        DataWorkerSharedPtr tmp;
        if(ChaosMetadataService::getInstance()->setting.worker_setting.log_metric) {
            INFO << "Enable caching worker log metric";
            INFO << "Init Device shared data worker metric";
            dsdwm_metric.reset(new worker::DeviceSharedDataWorkerMetric("DeviceSharedDataWorkerMetric",
                                                                        ChaosMetadataService::getInstance()->setting.worker_setting.log_metric_update_interval));
            
            tmp = ChaosMakeSharedPtr<worker::DeviceSharedDataWorkerMetricCollector>(dsdwm_metric);
        } else {
            tmp = ChaosMakeSharedPtr<chaos::metadata_service::worker::DeviceSharedDataWorker>();
        }
        device_data_worker.push_back(tmp);
        StartableService::initImplementation(*tmp, NULL, "DeviceSharedDataWorker", __PRETTY_FUNCTION__);
        StartableService::startImplementation(*tmp, "DeviceSharedDataWorker", __PRETTY_FUNCTION__);
        
    }
}

void QueryDataConsumer::start()  {}

void QueryDataConsumer::stop()  {}

void QueryDataConsumer::deinit()  {
    if(server_endpoint) {
        server_endpoint->releaseChannelInstance(device_channel);
        NetworkBroker::getInstance()->releaseDirectIOServerEndpoint(server_endpoint);
    }
    
    INFO << "Deallocating device push data worker list";
    for(int idx = 0; idx < ChaosMetadataService::getInstance()->setting.worker_setting.instances; idx++) {
        INFO << "Release device worker "<< idx;
        device_data_worker[idx]->stop();
        device_data_worker[idx]->deinit();
    }
    if(ChaosMetadataService::getInstance()->setting.worker_setting.log_metric) {
        dsdwm_metric.reset();
    }
}

#pragma mark DirectIODeviceServerChannelHandler
int QueryDataConsumer::consumePutEvent(const std::string& key,
                                       const uint8_t hst_tag,
                                       const ChaosStringSetConstSPtr meta_tag_set,
                                       BufferSPtr channel_data) {
    CHAOS_ASSERT(channel_data)
    int err = 0;
    
    DataServiceNodeDefinitionType::DSStorageType storage_type = static_cast<DataServiceNodeDefinitionType::DSStorageType>(hst_tag);
    //! if tag is == 1 the datapack is in liveonly
    
    if(storage_type & DataServiceNodeDefinitionType::DSStorageTypeLive) {
        //protected access to cached driver
        CacheDriver& cache_slot = DriverPoolManager::getInstance()->getCacheDrv();
        err = cache_slot.putData(key,
                                 channel_data);
        
    }
    
    if(storage_type & DataServiceNodeDefinitionType::DSStorageTypeHistory) {
        //compute the index to use for the data worker
        uint32_t index_to_use = device_data_worker_index++ % ChaosMetadataService::getInstance()->setting.worker_setting.instances;
        CHAOS_ASSERT(device_data_worker[index_to_use].get())
        //create storage job information
        auto job = ChaosMakeSharedPtr<DeviceSharedWorkerJob>();
        job->key = key;
        job->key_tag = hst_tag;
        job->data_pack = channel_data;
        job->meta_tag = MOVE(meta_tag_set);
        if((err = device_data_worker[index_to_use]->submitJobInfo(job))) {
            DEBUG_CODE(DBG << "Error pushing data into worker queue");
        }
    }
    return err;
}

int QueryDataConsumer::consumeHealthDataEvent(const std::string& key,
                                              const uint8_t hst_tag,
                                              const ChaosStringSetConstSPtr meta_tag_set,
                                              BufferSPtr channel_data) {
    int err = 0;
    
    CDataWrapper health_data_pack((char *)channel_data->data());
    health_data_pack.addInt64Value(NodeHealtDefinitionKey::NODE_HEALT_MDS_TIMESTAMP, TimingUtil::getTimeStamp());
    
    NodeDataAccess *s_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<NodeDataAccess>();
    
    HealthStatSDWrapper attribute_reference_wrapper;
    attribute_reference_wrapper.deserialize(&health_data_pack);
    if((err = s_da->setNodeHealthStatus(attribute_reference_wrapper().node_uid,
                                        attribute_reference_wrapper()))) {
        ERR << "error storing health data into database for key " << attribute_reference_wrapper().node_uid;
    }
    //create channel data with injected mds timestamp
    
    
    BufferSPtr channel_data_injected(health_data_pack.getBSONDataBuffer().release());
    return consumePutEvent(key,
                           hst_tag,
                           MOVE(meta_tag_set),
                           channel_data_injected);
}

int QueryDataConsumer::consumeDataCloudQuery(DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                                             const std::string& search_key,
                                             const ChaosStringSet& meta_tags,
                                             const uint64_t search_start_ts,
                                             const uint64_t search_end_ts,
                                             SearchSequence& last_element_found_seq,
                                             QueryResultPage& page_element_found) {
    
    int err = 0;
    //execute the query
    ObjectStorageDataAccess *obj_storage_da = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    if((err = obj_storage_da->findObject(search_key,
                                         meta_tags,
                                         search_start_ts,
                                         search_end_ts,
                                         query_header.field.record_for_page,
                                         page_element_found,
                                         last_element_found_seq))) {
        ERR << CHAOS_FORMAT("Error performing cloud query with code %1%", %err);
    }
    return err;
}


int QueryDataConsumer::consumeDataIndexCloudQuery(opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloud& query_header,
                                                  const std::string& search_key,
                                                  const ChaosStringSet& meta_tags,
                                                  const uint64_t search_start_ts,
                                                  const uint64_t search_end_ts,
                                                  opcode_headers::SearchSequence& last_element_found_seq,
                                                  opcode_headers::QueryResultPage& page_element_found) {
    int err = 0;
    //execute the query
    DataSearch search;
    search.key = search_key;
    search.meta_tags = meta_tags;
    search.timestamp_from = search_start_ts;
    search.timestamp_to = search_end_ts;
    search.page_len = query_header.field.record_for_page;
    ObjectStorageDataAccess *obj_storage_da = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    if((err = obj_storage_da->findObjectIndex(search,
                                              page_element_found,
                                              last_element_found_seq))) {
        ERR << CHAOS_FORMAT("Error performing cloud query with code %1%", %err);
    }
    return err;
}

int QueryDataConsumer::consumeGetEvent(chaos::common::data::BufferSPtr key_data,
                                       uint32_t key_len,
                                       opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult& result_header,
                                       chaos::common::data::BufferSPtr& result_value) {
    int err = 0;
    std::string key(key_data->data(),
                    key_data->size());
    //debug check
    //protected access to cached driver
    CacheDriver& cache_slot = DriverPoolManager::getInstance()->getCacheDrv();
    
    //get data
    err = cache_slot.getData(key,
                             result_value);
    if((err == 0 )&&
       result_value &&
       result_value->size()) {
        result_header.value_len = (uint32_t)result_value->size();
    }
    return err;
}

int QueryDataConsumer::consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode& header,
                                       const ChaosStringVector& keys,
                                       opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult& result_header,
                                       BufferSPtr& result_value,
                                       uint32_t& result_value_len) {
    int err = 0;
    //debug check
    //protected access to cached driver
    CacheDriver& cache_slot = DriverPoolManager::getInstance()->getCacheDrv();
    try{
        //get data
        DataBuffer data_buffer;
        MultiCacheData multi_cached_data;
        err = cache_slot.getData(keys,
                                 multi_cached_data);
        for(ChaosStringVectorConstIterator it = keys.begin(),
            end = keys.end();
            it != end;
            it++) {
            const CacheData& cached_element = multi_cached_data[*it];
            if(!cached_element ||
               cached_element->size() == 0) {
                //element has not been found so we need to create and empty cdata wrapper
                CDataWrapper tmp;
                int size = 0;
                const char * d_ptr = tmp.getBSONRawData(size);
                data_buffer.writeByte(d_ptr,
                                      size);
            } else {
                data_buffer.writeByte(cached_element->data(),
                                      (int32_t)cached_element->size());
            }
        }
        
        result_header.number_of_result = (uint32_t)multi_cached_data.size();
        result_value_len = data_buffer.getCursorLocation();
        result_value = ChaosMakeSharedPtr<Buffer>(data_buffer.release(), result_value_len, result_value_len, true);
        
    } catch(...) {}
    return err;
}

int QueryDataConsumer::getDataByIndex(const chaos::common::data::VectorCDWShrdPtr& indexes,
                                      chaos::common::data::VectorCDWShrdPtr& found_data) {
    
    ObjectStorageDataAccess *obj_storage_da = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    std::for_each(indexes.begin(), indexes.end(), [&obj_storage_da, &found_data](const CDWShrdPtr& index){
        int err = 0;
        CDWShrdPtr data;
        if((err = obj_storage_da->getObjectByIndex(index, data)) == 0) {
            found_data.push_back(data);
        } else {
            ERR << CHAOS_FORMAT("Error fetching data using index(%1%) with code %2%", %data->getJSONString()%err);
        }
    });
    return 0;
}

int QueryDataConsumer::consumeDataCloudDelete(const std::string& search_key,
                                              uint64_t start_ts,
                                              uint64_t end_ts){
    int err = 0;
    VectorObject reuslt_object_found;
    ObjectStorageDataAccess *obj_storage_da = DriverPoolManager::getInstance()->getObjectStorageDrv().getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    if((err = obj_storage_da->deleteObject(search_key,
                                           start_ts,
                                           end_ts))) {
        ERR << CHAOS_FORMAT("Error performing cloud query with code %1%", %err);
    }
    return err;
}

#pragma mark DirectIOSystemAPIServerChannelHandler
// Return the dataset for a producerkey ona specific snapshot
int QueryDataConsumer::consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader& header,
                                                      const std::string& producer_id,
                                                      chaos::common::data::BufferSPtr& channel_found_data,
                                                      DirectIOSystemAPISnapshotResultHeader &result_header) {
    int err = 0;
    std::string channel_type;
    //CHAOS_ASSERT(api_result)
    SnapshotDataAccess *s_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<SnapshotDataAccess>();
    
    //trduce int to postfix channel type
    switch(header.field.channel_type) {
        case 0:
            channel_type = DataPackPrefixID::OUTPUT_DATASET_POSTFIX;
            break;
        case 1:
            channel_type = DataPackPrefixID::INPUT_DATASET_POSTFIX;
            break;
        case 2:
            channel_type = DataPackPrefixID::CUSTOM_DATASET_POSTFIX;
            break;
        case 3:
            channel_type = DataPackPrefixID::SYSTEM_DATASET_POSTFIX;
            break;
            
    }
    
    if((err = s_da->snapshotGetDatasetForProducerKey(header.field.snap_name,
                                                     producer_id,
                                                     channel_type,
                                                     channel_found_data))) {
        std::strcpy(result_header.error_message, "Error retriving the snapshoted dataaset for producer key");
        ERR << result_header.error_message << "[" << header.field.snap_name << "/" << producer_id<<"]";
    } else {
        if(channel_found_data &&
           channel_found_data->size()) {
            result_header.error = 0;
            std::strcpy(result_header.error_message, "Snapshot found");
        } else {
            result_header.error = -2;
            std::strcpy(result_header.error_message, "Channel data not found in snapshot");
            
        }
    }
    return err;
}

int QueryDataConsumer::consumeLogEntries(const std::string& node_name,
                                         const ChaosStringVector& log_entries) {
    int err = 0;
    for(ChaosStringVectorConstIterator it = log_entries.begin(),
        end = log_entries.end();
        it != end;
        it++) {
        AgentDataAccess *a_da = DriverPoolManager::getInstance()->getPersistenceDataAccess<AgentDataAccess>();
        if((err = a_da->pushLogEntry(node_name, *it))){
            ERR << CHAOS_FORMAT("Error push entry for node %1%", %node_name);
        }
    }
    return err;
}

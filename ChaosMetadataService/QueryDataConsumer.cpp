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

using namespace chaos::data_service;
using namespace chaos::data_service::cache_system;
using namespace chaos::data_service::object_storage::abstraction;

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
device_data_worker_index(0),
object_storage_driver(NULL){}

QueryDataConsumer::~QueryDataConsumer() {}

void QueryDataConsumer::init(void *init_data) throw (chaos::CException) {
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
    
    //get local object storage driver
    object_storage_driver = ObjectFactoryRegister<service_common::persistence::data_access::AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(ChaosMetadataService::getInstance()->setting.object_storage_setting.driver_impl+"ObjectStorageDriver");
    if(!object_storage_driver) throw chaos::CException(-1, "No Object Storage driver found", __PRETTY_FUNCTION__);
    InizializableService::initImplementation(object_storage_driver, NULL, object_storage_driver->getName(), __PRETTY_FUNCTION__);
    
    //Shared data worker
    if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric) {
        INFO << "Init Device shared data worker metric";
        dsdwm_metric.reset(new worker::DeviceSharedDataWorkerMetric("DeviceSharedDataWorkerMetric",
                                                                    ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric_update_interval));
    }
    //device data worker instances
    chaos::data_service::worker::DeviceSharedDataWorker *tmp = NULL;
    device_data_worker = (chaos::data_service::worker::DataWorker**) malloc(sizeof(chaos::data_service::worker::DataWorker**) * ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_num);
    if(!device_data_worker) throw chaos::CException(-5, "Error allocating device workers", __FUNCTION__);
    for(int idx = 0;
        idx < ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_num;
        idx++) {
        
        if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric) {
            INFO << "Enable caching worker log metric";
            //install the data worker taht grab the metric
            device_data_worker[idx] = tmp = new worker::DeviceSharedDataWorkerMetricCollector(dsdwm_metric);
        } else {
            device_data_worker[idx] = tmp = new chaos::data_service::worker::DeviceSharedDataWorker();
        }
        tmp->init(NULL);
        tmp->start();
    }
}

void QueryDataConsumer::start() throw (chaos::CException) {
}

void QueryDataConsumer::stop() throw (chaos::CException) {
}

void QueryDataConsumer::deinit() throw (chaos::CException) {
    if(server_endpoint) {
        server_endpoint->releaseChannelInstance(device_channel);
        NetworkBroker::getInstance()->releaseDirectIOServerEndpoint(server_endpoint);
    }
    
    INFO << "Deallocating device push data worker list";
    for(int idx = 0; idx < ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_num; idx++) {
        INFO << "Release device worker "<< idx;
        device_data_worker[idx]->stop();
        device_data_worker[idx]->deinit();
        DELETE_OBJ_POINTER(device_data_worker[idx])
    }
    free(device_data_worker);
    if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric) {
        dsdwm_metric.reset();
    }
    
    if(object_storage_driver){
        InizializableService::deinitImplementation(object_storage_driver, object_storage_driver->getName(), __PRETTY_FUNCTION__);
        delete(object_storage_driver);
    }
}

#pragma mark DirectIODeviceServerChannelHandler
int QueryDataConsumer::consumePutEvent(DirectIODeviceChannelHeaderPutOpcode *header,
                                       void *channel_data,
                                       uint32_t channel_data_len) {
    CHAOS_ASSERT(header)
    CHAOS_ASSERT(channel_data)
    int err = 0;
    DataServiceNodeDefinitionType::DSStorageType storage_type = static_cast<DataServiceNodeDefinitionType::DSStorageType>(header->tag);
    //! if tag is == 1 the datapack is in liveonly
    bool send_to_storage_layer = (storage_type != DataServiceNodeDefinitionType::DSStorageTypeLive &&
                                  storage_type != DataServiceNodeDefinitionType::DSStorageTypeUndefined);
    switch(storage_type) {
        case DataServiceNodeDefinitionType::DSStorageTypeLiveHistory:
        case DataServiceNodeDefinitionType::DSStorageTypeLive:{
            CacheData cache_data((char*)channel_data, ((char*)channel_data)+channel_data_len);
            //protected access to cached driver
            CachePoolSlot *cache_slot = DriverPoolManager::getInstance()->getCacheDriverInstance();
            if(cache_slot) {
                err = cache_slot->resource_pooled->putData(std::string((const char *)GET_PUT_OPCODE_KEY_PTR(header),
                                                                       header->key_len),
                                                           cache_data);
                DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
            } else {
                ERR << "Error allocating cache slot";
            }
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeHistory:
        case DataServiceNodeDefinitionType::DSStorageTypeUndefined:
            break;
            
        default: {
            ERR << "Bad storage tag: " << header->tag;
            break;
        }
    }
    
    if(send_to_storage_layer) {
        //compute the index to use for the data worker
        uint32_t index_to_use = device_data_worker_index++ % ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_num;
        CHAOS_ASSERT(device_data_worker[index_to_use])
        //create storage job information
        chaos::data_service::worker::DeviceSharedWorkerJob *job = new chaos::data_service::worker::DeviceSharedWorkerJob();
        job->request_header = header;
        job->data_pack = channel_data;
        job->data_pack_len = channel_data_len;
        if((err = device_data_worker[index_to_use]->submitJobInfo(job))) {
            DEBUG_CODE(DBG << "error pushing data into worker queue");
            delete job;
            free(header);
            free(channel_data);
        }
    } else {
        free(header);
        free(channel_data);
    }
    return err;
}

int QueryDataConsumer::consumeHealthDataEvent(DirectIODeviceChannelHeaderPutOpcode *header,
                                              void *channel_data,
                                              uint32_t channel_data_len) {
    int err = 0;
    
    CDataWrapper health_data_pack((char *)channel_data);
    
    NodeDataAccess *s_da = PersistenceManager::getInstance()->getDataAccess<NodeDataAccess>();
    
    HealthStatSDWrapper attribute_reference_wrapper;
    attribute_reference_wrapper.deserialize(&health_data_pack);
    if((err = s_da->setNodeHealthStatus(attribute_reference_wrapper().node_uid,
                                        attribute_reference_wrapper()))) {
        ERR << "error storing health data into database for key " << attribute_reference_wrapper().node_uid;
    }
    return consumePutEvent(header,
                           channel_data,
                           channel_data_len);
}

int QueryDataConsumer::consumeDataCloudQuery(DirectIODeviceChannelHeaderOpcodeQueryDataCloud *query_header,
                                             const std::string& search_key,
                                             uint64_t search_start_ts,
                                             uint64_t search_end_ts,
                                             SearchSequence& last_element_found_seq,
                                             QueryResultPage& page_element_found) {
    
    int err = 0;
    //execute the query
    ObjectStorageDataAccess *obj_storage_da = object_storage_driver->getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    if((err = obj_storage_da->findObject(search_key,
                                         search_start_ts,
                                         search_end_ts,
                                         query_header->field.record_for_page,
                                         page_element_found,
                                         last_element_found_seq))) {
        ERR << CHAOS_FORMAT("Error performing cloud query with code %1%", %err);
    }
    return err;
}

int QueryDataConsumer::consumeGetEvent(DirectIODeviceChannelHeaderGetOpcode *header,
                                       void *channel_data,
                                       uint32_t channel_data_len,
                                       opcode_headers::DirectIODeviceChannelHeaderGetOpcodeResult *result_header,
                                       void **result_value) {
    int err = 0;
    //debug check
    //protected access to cached driver
    CachePoolSlot *cache_slot = DriverPoolManager::getInstance()->getCacheDriverInstance();
    try{
        CacheData cache_data;
        //get data
        err = cache_slot->resource_pooled->getData(std::string((const char*)channel_data,
                                                               channel_data_len),
                                                   cache_data);
        if(cache_slot) {
            if(cache_data.size()) {
                result_header->value_len = (uint32_t)cache_data.size();
                *result_value = malloc(result_header->value_len);
                CHAOS_ASSERT(*result_value)
                std::memcpy(*result_value, &cache_data[0], result_header->value_len);
            }
        } else {
            ERR << "Error allocating cache slot";
        }
    } catch(...) {}
    DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
    if(channel_data) free(channel_data);
    if(header) free(header);
    return err;
}

int QueryDataConsumer::consumeGetEvent(opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcode *header,
                                       const ChaosStringVector& keys,
                                       opcode_headers::DirectIODeviceChannelHeaderMultiGetOpcodeResult *result_header,
                                       void **result_value,
                                       uint32_t& result_value_len) {
    int err = 0;
    //debug check
    //protected access to cached driver
    CachePoolSlot *cache_slot = DriverPoolManager::getInstance()->getCacheDriverInstance();
    try{
        //get data
        DataBuffer<> data_buffer;
        MultiCacheData multi_cached_data;
        err = cache_slot->resource_pooled->getData(keys,
                                                   multi_cached_data);
        if(cache_slot) {
            for(ChaosStringVectorConstIterator it = keys.begin(),
                end = keys.end();
                it != end;
                it++) {
                const CacheData& cached_element = multi_cached_data[*it];
                if(cached_element.size() == 0) {
                    //element has not been found so we need to create and empty cdata wrapper
                    CDataWrapper tmp;
                    int size = 0;
                    const char * d_ptr = tmp.getBSONRawData(size);
                    data_buffer.writeByte(d_ptr,
                                          size);
                } else {
                    data_buffer.writeByte(&cached_element[0],
                                          (int32_t)cached_element.size());
                }
            }
            
            result_header->number_of_result = (uint32_t)multi_cached_data.size();
            result_value_len = data_buffer.getCursorLocation();
            *result_value = data_buffer.release();
        } else {
            ERR << "Error allocating cache slot";
        }
    } catch(...) {}
    DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
    if(header) free(header);
    return err;
}

int QueryDataConsumer::consumeDataCloudDelete(const std::string& search_key,
                                              uint64_t start_ts,
                                              uint64_t end_ts){
    int err = 0;
    //execute the query
    VectorObject reuslt_object_found;
    ObjectStorageDataAccess *obj_storage_da = object_storage_driver->getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    if((err = obj_storage_da->deleteObject(search_key,
                                           start_ts,
                                           end_ts))) {
        ERR << CHAOS_FORMAT("Error performing lcoud query with code %1%", %err);
    }
    return err;
}

#pragma mark DirectIOSystemAPIServerChannelHandler
// Return the dataset for a producerkey ona specific snapshot
int QueryDataConsumer::consumeGetDatasetSnapshotEvent(opcode_headers::DirectIOSystemAPIChannelOpcodeNDGSnapshotHeader *header,
                                                      const std::string& producer_id,
                                                      void **channel_found_data,
                                                      uint32_t& channel_found_data_length,
                                                      DirectIOSystemAPISnapshotResultHeader& api_result) {
    int err = 0;
    std::string channel_type;
    //debug check
    CHAOS_ASSERT(header)
    //CHAOS_ASSERT(api_result)
    SnapshotDataAccess *s_da = PersistenceManager::getInstance()->getDataAccess<SnapshotDataAccess>();
    
    //trduce int to postfix channel type
    switch(header->field.channel_type) {
        case 0:
            channel_type = DataPackPrefixID::OUTPUT_DATASET_POSTFIX
            ;
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
    
    if((err = s_da->snapshotGetDatasetForProducerKey(header->field.snap_name,
                                                     producer_id,
                                                     channel_type,
                                                     channel_found_data,
                                                     channel_found_data_length))) {
        api_result.error = err;
        std::strcpy(api_result.error_message, "Error retriving the snapshoted dataaset for producer key");
        ERR << api_result.error_message << "[" << header->field.snap_name << "/" << producer_id<<"]";
    } else {
        if(*channel_found_data) {
            api_result.error = 0;
            std::strcpy(api_result.error_message, "Snapshot found");
        } else {
            api_result.error = -2;
            std::strcpy(api_result.error_message, "Channel data not found in snapshot");
            
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
        AgentDataAccess *a_da = PersistenceManager::getInstance()->getDataAccess<AgentDataAccess>();
        if((err = a_da->pushLogEntry(node_name, *it))){
            ERR << CHAOS_FORMAT("Error push entry for node %1%", %node_name);
        }
    }
    return err;
}

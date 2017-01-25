/*
 *	QueryDataConsumer.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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
#include <chaos/common/utility/InetUtility.h>

#include <boost/thread.hpp>

using namespace chaos::metadata_service;

using namespace chaos::data_service;
using namespace chaos::data_service::object_storage::abstraction;

using namespace chaos::metadata_service::persistence;
using namespace chaos::metadata_service::persistence::data_access;

using namespace chaos::common::data::structured;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define QDCAPP_ INFO_LOG(QueryDataConsumer)
#define QDCDBG_ DBG_LOG(QueryDataConsumer)
#define QDCERR_ ERR_LOG(QueryDataConsumer)

boost::shared_ptr<worker::DeviceSharedDataWorkerMetric> dsdwm_metric;

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
    QDCAPP_ << "QueryDataConsumer initialized with endpoint "<< server_endpoint->getRouteIndex();
    
    QDCAPP_ << "Allocating DirectIODeviceServerChannel";
    device_channel = (DirectIODeviceServerChannel*)server_endpoint->getNewChannelInstance("DirectIODeviceServerChannel");
    if(!device_channel) throw chaos::CException(-3, "Error allocating device server channel", __FUNCTION__);
    device_channel->setHandler(this);
    
    QDCAPP_ << "Allocating DirectIOSystemAPIServerChannel";
    system_api_channel = (DirectIOSystemAPIServerChannel*)server_endpoint->getNewChannelInstance("DirectIOSystemAPIServerChannel");
    if(!system_api_channel) throw chaos::CException(-4, "Error allocating system api server channel", __FUNCTION__);
    system_api_channel->setHandler(this);
    
    //get local object storage driver
    object_storage_driver = ObjectFactoryRegister<service_common::persistence::data_access::AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(ChaosMetadataService::getInstance()->setting.object_storage_setting.driver_impl+"ObjectStorageDriver");
    if(!object_storage_driver) throw chaos::CException(-1, "No Object Storage driver found", __PRETTY_FUNCTION__);
    InizializableService::initImplementation(object_storage_driver, NULL, object_storage_driver->getName(), __PRETTY_FUNCTION__);
    
    //Shared data worker
    if(ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_log_metric) {
        QDCAPP_ << "Init Device shared data worker metric";
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
            QDCAPP_ << "Enable caching worker log metric";
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
    
    QDCAPP_ << "Deallocating device push data worker list";
    for(int idx = 0; idx < ChaosMetadataService::getInstance()->setting.cache_driver_setting.caching_worker_num; idx++) {
        QDCAPP_ << "Release device worker "<< idx;
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
            //protected access to cached driver
            CachePoolSlot *cache_slot = DriverPoolManager::getInstance()->getCacheDriverInstance();
            err = cache_slot->resource_pooled->putData(GET_PUT_OPCODE_KEY_PTR(header),
                                                       header->key_len,
                                                       channel_data,
                                                       channel_data_len);
            DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
            break;
        }
        case DataServiceNodeDefinitionType::DSStorageTypeHistory:
        case DataServiceNodeDefinitionType::DSStorageTypeUndefined:
            break;
            
        default: {
            QDCERR_ << "Bad storage tag: " << header->tag;
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
            DEBUG_CODE(QDCDBG_ << "error pushing data into worker queue");
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
        QDCERR_ << "error storing health data into database for key " << attribute_reference_wrapper().node_uid;
    }
    return consumePutEvent(header,
                           channel_data,
                           channel_data_len);
}

int QueryDataConsumer::consumeDataCloudQuery(DirectIODeviceChannelHeaderOpcodeQueryDataCloud *query_header,
                                             const std::string& search_key,
                                             uint64_t search_start_ts,
                                             uint64_t search_end_ts,
                                             uint64_t last_sequence_id,
                                             DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult * result_header,
                                             void **result_value) {
    
    int err = 0;
    //execute the query
    VectorObject reuslt_object_found;
    ObjectStorageDataAccess *obj_storage_da = object_storage_driver->getDataAccess<object_storage::abstraction::ObjectStorageDataAccess>();
    if((err = obj_storage_da->findObject(search_key,
                                         search_start_ts,
                                         search_end_ts,
                                         query_header->field.record_for_page,
                                         reuslt_object_found,
                                         (result_header->last_found_sequence = last_sequence_id)))) {
        QDCERR_ << CHAOS_FORMAT("Error performing cloud query with code %1%", %err);
    } else if(reuslt_object_found.size()){
        //we successfully have perform query
        result_header->result_data_size = 0;
        result_header->numer_of_record_found = reuslt_object_found.size();
        for(VectorObjectIterator it = reuslt_object_found.begin(),
            end = reuslt_object_found.end();
            it != end;
            it++) {
            //write result into mresults memory
            int element_bson_size = 0;
            const char * element_bson_mem = (*it)->getBSONRawData(element_bson_size);
            
            //enlarge buffer
            *result_value = std::realloc(*result_value, (result_header->result_data_size + element_bson_size));
            
            //copy bson elelment in memory location
            char *mem_start_copy = ((char*)*result_value)+result_header->result_data_size;
            
            //copy
            std::memcpy(mem_start_copy, element_bson_mem, element_bson_size);
            
            //keep track of the full size of the result
            result_header->result_data_size +=element_bson_size;
        }
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
        //get data
        err = cache_slot->resource_pooled->getData(channel_data,
                                                   channel_data_len,
                                                   result_value,
                                                   result_header->value_len);
    } catch(...) {
        
    }
    DriverPoolManager::getInstance()->releaseCacheDriverInstance(cache_slot);
    if(channel_data) free(channel_data);
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
        QDCERR_ << CHAOS_FORMAT("Error performing lcoud query with code %1%", %err);
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
        QDCERR_ << api_result.error_message << "[" << header->field.snap_name << "/" << producer_id<<"]";
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
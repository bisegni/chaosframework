/*
 *	IODirectIODriver.cpp
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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common//global.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>

#define IODirectIODriver_LOG_HEAD "[IODirectIODriver] - "

#define IODirectIODriver_LINFO_ INFO_LOG(IODirectIODriver)
#define IODirectIODriver_DLDBG_ DBG_LOG(IODirectIODriver)
#define IODirectIODriver_LERR_ ERR_LOG(IODirectIODriver)

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::utility;

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace chaos_data = chaos::common::data;
namespace chaos_dio = chaos::common::direct_io;
namespace chaos_dio_channel = chaos::common::direct_io::channel;

DEFINE_CLASS_FACTORY(IODirectIODriver, IODataDriver);

//using namespace memcache;

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
IODirectIODriver::IODirectIODriver(std::string alias):
NamedService(alias),
current_endpoint_p_port(0),
current_endpoint_s_port(0),
current_endpoint_index(0),
connectionFeeder(alias, this),
uuid(UUIDUtil::generateUUIDLite()){
    //clear
    std::memset(&init_parameter, 0, sizeof(IODirectIODriverInitParam));
    
    device_server_channel = NULL;
    
    read_write_index = 0;
    data_cache.data_ptr = NULL;
    data_cache.data_len = 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
IODirectIODriver::~IODirectIODriver() {
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::setDirectIOParam(IODirectIODriverInitParam& _init_parameter) {
    //store the configuration
    init_parameter = _init_parameter;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::init(void *_init_parameter) throw(CException) {
    IODataDriver::init(_init_parameter);
    
    IODirectIODriver_LINFO_ << "Check init parameter";
    
    if(!init_parameter.network_broker) throw CException(-1, "No network broker configured", __PRETTY_FUNCTION__);
    
    init_parameter.client_instance = init_parameter.network_broker->getSharedDirectIOClientInstance();
    //if(!init_parameter.client_instance) throw CException(-1, "No client configured", __PRETTY_FUNCTION__);
    
    init_parameter.endpoint_instance = init_parameter.network_broker->getDirectIOServerEndpoint();
    if(!init_parameter.endpoint_instance) throw CException(-1, "No endpoint configured", __PRETTY_FUNCTION__);
    
    //initialize client
    //InizializableService::initImplementation(init_parameter.client_instance, _init_parameter, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
    
    //get the client and server channel
    IODirectIODriver_LINFO_ << "Allocate the default device server channel";
    device_server_channel = (chaos_dio_channel::DirectIODeviceServerChannel *)init_parameter.endpoint_instance->getNewChannelInstance("DirectIODeviceServerChannel");
    device_server_channel->setHandler(this);
    
    //store endpoint idnex for fast access
    current_endpoint_p_port = init_parameter.endpoint_instance->getPublicServerInterface()->getPriorityPort();
    current_endpoint_s_port = init_parameter.endpoint_instance->getPublicServerInterface()->getServicePort();
    current_endpoint_index = init_parameter.endpoint_instance->getRouteIndex();
    IODirectIODriver_LINFO_ << "Our receiving priority port is " << current_endpoint_p_port << " and enpoint is " <<current_endpoint_index;
    
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::deinit() throw(CException) {
    IODirectIODriver_LINFO_ << "Remove active query";
    //acquire write lock on map
    boost::unique_lock<boost::shared_mutex> wmap_loc(map_query_future_mutex);
    
    //scan all remained query
    for(std::map<string, QueryFuture*>::iterator it = map_query_future.begin();
        it != map_query_future.end();
        it++) {
        releaseQuery(it->second);
    }
    map_query_future.clear();
    
    if(data_cache.data_ptr) {
        IODirectIODriver_LINFO_ << "delete last data received";
        free(data_cache.data_ptr);
    }
    
    //remove all url and service
    IODirectIODriver_LINFO_ << "Remove all urls";
    connectionFeeder.clear();
    
    //initialize client
    //InizializableService::deinitImplementation(init_parameter.client_instance, init_parameter.client_instance->getName(), __PRETTY_FUNCTION__);
    //delete(init_parameter.client_instance);
    
    //deinitialize server channel
    if(device_server_channel) {
        init_parameter.endpoint_instance->releaseChannelInstance(device_server_channel);
    }
    
    if(init_parameter.endpoint_instance) {
        init_parameter.network_broker->releaseDirectIOServerEndpoint(init_parameter.endpoint_instance);
    }
    IODataDriver::deinit();
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::storeRawData(const std::string& key,
                                    chaos::common::data::SerializationBuffer *serialization,
                                    int store_hint)  throw(CException) {
    CHAOS_ASSERT(serialization)
    int err = 0;
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    //if(next_client->connection->getState() == chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected)
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    serialization->disposeOnDelete = !next_client;
    if(next_client) {
        //free the packet
        serialization->disposeOnDelete = false;
        if((err = (int)next_client->device_client_channel->storeAndCacheDataOutputChannel(key,
                                                                                          (void*)serialization->getBufferPtr(),
                                                                                          (uint32_t)serialization->getBufferLen(),
                                                                                          (direct_io::channel::DirectIODeviceClientChannelPutMode)store_hint))) {
            IODirectIODriver_LERR_ << "Error storing data into data service "<<next_client->connection->getServerDescription()<<" with code:" << err;
        }
    } else {
        DEBUG_CODE(IODirectIODriver_DLDBG_ << "No available socket->loose packet");
    }
    delete(serialization);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
char* IODirectIODriver::retriveRawData(const std::string& key, size_t *dim)  throw(CException) {
    char* result = NULL;
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return NULL;
    
    uint32_t size =0;
    int err = (int)next_client->device_client_channel->requestLastOutputData(key, (void**)&result, size);
    if(err) {
        IODirectIODriver_LERR_ << "Error retriving data from data service "<<next_client->connection->getServerDescription()<< " with code:" << err;
    } else {
        *dim = (size_t)size;
    }
    return result;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
//! restore from a tag a dataset associated to a key
int IODirectIODriver::loadDatasetTypeFromSnapshotTag(const std::string& restore_point_tag_name,
                                                     const std::string& key,
                                                     uint32_t dataset_type,
                                                     chaos_data::CDataWrapper **cdatawrapper_handler) {
    int err = 0;
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return 0;
    chaos_dio_channel::DirectIOSystemAPIGetDatasetSnapshotResultPtr snapshot_result = NULL;
    if((err = (int)next_client->system_client_channel->getDatasetSnapshotForProducerKey(restore_point_tag_name,
                                                                                        key,
                                                                                        dataset_type,
                                                                                        &snapshot_result))) {
        IODirectIODriver_LERR_ << "Error loading the dataset type:"<<dataset_type<< " for key:" << key << " from restor point:" <<restore_point_tag_name;
    } else {
        if(snapshot_result &&
           snapshot_result->channel_data) {
            //we have the dataaset
            try {
                *cdatawrapper_handler = new chaos_data::CDataWrapper((const char*)snapshot_result->channel_data);
                IODirectIODriver_LINFO_ << "Got dataset type:"<<dataset_type<< " for key:" << key << " from snapshot tag:" <<restore_point_tag_name;
            } catch (std::exception& ex) {
                IODirectIODriver_LERR_ << "Error deserializing the dataset type:"<<dataset_type<< " for key:" << key << " from snapshot tag:" <<restore_point_tag_name << " with error:" << ex.what();
            } catch (...) {
                IODirectIODriver_LERR_ << "Error deserializing the dataset type:"<<dataset_type<< " for key:" << key << " from snapshot tag:" <<restore_point_tag_name;
            }
            free(snapshot_result->channel_data);
        }
    }
    //delete the received result if there was one
    if(snapshot_result) free(snapshot_result);
    return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
//! restore from a tag a dataset associated to a key
int IODirectIODriver::createNewSnapshot(const std::string& snapshot_tag,
                                        const std::vector<std::string>& node_list) {
    int err = 0;
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return 0;
    chaos_dio_channel::DirectIOSystemAPIGetDatasetSnapshotResultPtr snapshot_result = NULL;
    
    if((err = (int)next_client->system_client_channel->makeNewDatasetSnapshot(snapshot_tag,
                                                                              node_list,
                                                                              &snapshot_result))) {
        IODirectIODriver_LERR_ << "Error creating snapshot:"<<snapshot_tag << " with error:" <<err;
    } else {
        if(snapshot_result &&
           snapshot_result->api_result.error) {
            err = snapshot_result->api_result.error;
            //we have the dataaset
            IODirectIODriver_LERR_ << "Error creating snapshot:"<<snapshot_tag << " with error:" <<snapshot_result->api_result.error << " and message:" << snapshot_result->api_result.error_message;
        }
    }
    //delete the received result if there was one
    if(snapshot_result) free(snapshot_result);
    return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int IODirectIODriver::deleteSnapshot(const std::string& snapshot_tag) {
    int err = 0;
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return 0;
    chaos_dio_channel::DirectIOSystemAPIGetDatasetSnapshotResultPtr snapshot_result = NULL;
    
    if((err = (int)next_client->system_client_channel->deleteDatasetSnapshot(snapshot_tag,
                                                                             &snapshot_result))) {
        IODirectIODriver_LERR_ << "Error erasing snapshot:"<<snapshot_tag << " with error:" <<err;
    } else {
        if(snapshot_result &&
           snapshot_result->api_result.error) {
            err = snapshot_result->api_result.error;
            //we have the dataaset
            IODirectIODriver_LERR_ << "Error erasing snapshot:"<<snapshot_tag << " with error:" <<snapshot_result->api_result.error << " and message:" << snapshot_result->api_result.error_message;
        }
    }
    //delete the received result if there was one
    if(snapshot_result) free(snapshot_result);
    return err;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::addServerURL(const std::string& url) {
    if(!common::direct_io::DirectIOClient::checkURL(url)) {
        IODirectIODriver_LERR_ << "Url " << url << " non well formed";
        return;
    }
    IODirectIODriver_LINFO_ << "Adding url" << url;
    //add new url to connection feeder
    connectionFeeder.addURL(chaos::common::network::URL(url));
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
chaos::common::data::CDataWrapper* IODirectIODriver::updateConfiguration(chaos::common::data::CDataWrapper* newConfigration) {
    //lock the feeder access
    boost::unique_lock<boost::shared_mutex>(mutext_feeder);
    //checkif someone has passed us the device indetification
    if(newConfigration->hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST)){
        IODirectIODriver_LINFO_ << "Get the DataManager LiveData address value";
        auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> liveMemAddrConfig(newConfigration->getVectorValue(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST));
        size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
        for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
            string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
            if(!common::direct_io::DirectIOClient::checkURL(serverDesc)) {
                IODirectIODriver_DLDBG_ << "Data proxy server description " << serverDesc << " non well formed";
                continue;
            }
            //add new url to connection feeder
            connectionFeeder.addURL(chaos::common::network::URL(serverDesc));
        }
    }
    return NULL;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::disposeService(void *service_ptr) {
    if(!service_ptr) return;
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(service_ptr);
    
    if(next_client->system_client_channel) next_client->connection->releaseChannelInstance(next_client->system_client_channel);
    if(next_client->device_client_channel) next_client->connection->releaseChannelInstance(next_client->device_client_channel);
    init_parameter.client_instance->releaseConnection(next_client->connection);
    delete(next_client);
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void* IODirectIODriver::serviceForURL(const common::network::URL& url, uint32_t service_index) {
    IODirectIODriver_LINFO_ << "try to add connection for " << url.getURL();
    IODirectIODriverClientChannels * clients_channel = NULL;
    chaos_direct_io::DirectIOClientConnection *tmp_connection = init_parameter.client_instance->getNewConnection(url.getURL());
    if(tmp_connection) {
        clients_channel = new IODirectIODriverClientChannels();
        clients_channel->connection = tmp_connection;
        
        //allocate the client channel
        clients_channel->device_client_channel = (chaos_dio_channel::DirectIODeviceClientChannel*)tmp_connection->getNewChannelInstance("DirectIODeviceClientChannel");
        if(!clients_channel->device_client_channel) {
            IODirectIODriver_LERR_ << "Error creating client device channel for " << url.getURL();
            
            //release conenction
            init_parameter.client_instance->releaseConnection(tmp_connection);
            
            //relase struct
            delete(clients_channel);
            return NULL;
        }
        
        clients_channel->system_client_channel = (chaos_dio_channel::DirectIOSystemAPIClientChannel*)tmp_connection->getNewChannelInstance("DirectIOSystemAPIClientChannel");
        if(!clients_channel->system_client_channel) {
            IODirectIODriver_LERR_ << "Error creating client system api channel for " << url.getURL();
            
            //releasing device channel
            tmp_connection->releaseChannelInstance(clients_channel->device_client_channel);
            
            //release connection
            init_parameter.client_instance->releaseConnection(tmp_connection);
            //relase struct
            delete(clients_channel);
            return NULL;
        }
        //set the answer information
        clients_channel->device_client_channel->setAnswerServerInfo(current_endpoint_p_port, current_endpoint_s_port, current_endpoint_index);
        
        //set this driver instance as event handler for connection
        clients_channel->connection->setEventHandler(this);
        clients_channel->connection->setCustomStringIdentification(boost::lexical_cast<std::string>(service_index));
    } else {
        IODirectIODriver_LERR_ << "Error creating client connection for " << url.getURL();
    }
    IODirectIODriver_LINFO_ << "connection for " << url.getURL() << " added succesfully";
    return clients_channel;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
                                   chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
    //if the channel has bee disconnected turn the relative index offline, if onli reput it online
    boost::unique_lock<boost::shared_mutex>(mutext_feeder);
    uint32_t service_index = boost::lexical_cast<uint32_t>(client_connection->getCustomStringIdentification());
    switch(event) {
        case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:
            DEBUG_CODE(IODirectIODriver_LINFO_ << "Manage Connected event to service with index " << service_index << " and url" << client_connection->getURL();)
            connectionFeeder.setURLOnline(service_index);
            break;
            
        case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:
            DEBUG_CODE(IODirectIODriver_LINFO_ << "Manage Disconnected event for service with index " << service_index << " and url" << client_connection->getURL();)
            connectionFeeder.setURLOffline(service_index);
            break;
    }
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
QueryFuture *IODirectIODriver::performQuery(const std::string& key,
                                            uint64_t start_ts,
                                            uint64_t end_ts) {
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return NULL;
    
    QueryFuture *q = NULL;
    std::string query_id;
    if(!next_client->device_client_channel->queryDataCloud(key, start_ts, end_ts, query_id)) {
        //acquire write lock
        boost::unique_lock<boost::shared_mutex> wmap_loc(map_query_future_mutex);
        q = _getNewQueryFutureForQueryID(query_id);
        if(q) {
            //add query to map
            map_query_future.insert(make_pair<string, QueryFuture*>(query_id, q));
        } else {
            releaseQuery(q);
        }
    }
    return q;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void IODirectIODriver::releaseQuery(QueryFuture *query_future) {
    //acquire write lock
    boost::unique_lock<boost::shared_mutex> wmap_loc(map_query_future_mutex);
    if(map_query_future.count(query_future->getQueryID())) {
        map_query_future.erase(query_future->getQueryID());
    }
    //delete query
    IODataDriver::releaseQuery(query_future);
}


/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int IODirectIODriver::consumeDataCloudQueryStartResult(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudStartResult *header) {
    
    //get the query id
    std::string query_id(header->field.query_id, 8);
    
    //acquire read lock
    boost::shared_lock<boost::shared_mutex> rmap_loc(map_query_future_mutex);
    
    //get qeury iterator on map
    std::map<string, QueryFuture*>::iterator it = map_query_future.find(query_id);
    if(it != map_query_future.end()) {
        //initialize the query for for the result receivement
        _startQueryFutureResult(*it->second, header->field.total_element_found);
    }
    return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int IODirectIODriver::consumeDataCloudQueryResult(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudResult *header,
                                                  void *data_found,
                                                  uint32_t data_lenght){
    std::string query_id(header->field.query_id, 8);
    
    //acquire read lock
    boost::shared_lock<boost::shared_mutex> rmap_loc(map_query_future_mutex);
    
    //get qeury iterator on map
    std::map<string, QueryFuture*>::iterator it = map_query_future.find(query_id);
    if(it != map_query_future.end()) {
        try{
            chaos_data::CDataWrapper *data_pack = new chaos_data::CDataWrapper((char *)data_found);
            //we have map so we will add the new packet
            _pushResultToQueryFuture(*it->second, data_pack, header->field.element_index);
        }catch(...) {
            IODirectIODriver_LERR_ << "error parsing reuslt data pack";
        }
    }
    
    //deelte received data
    free(data_found);
    free(header);
    return 0;
}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
int IODirectIODriver::consumeDataCloudQueryEndResult(chaos_dio_channel::opcode_headers::DirectIODeviceChannelHeaderOpcodeQueryDataCloudEndResult *header,
                                                     void *error_message_string_data,
                                                     uint32_t error_message_string_data_length) {
    int err = 0;
    //get the query id
    std::string query_id(header->field.query_id, 8);
    
    //acquire read lock
    boost::shared_lock<boost::shared_mutex> rmap_loc(map_query_future_mutex);
    
    //get qeury iterator on map
    std::map<string, QueryFuture*>::iterator it = map_query_future.find(query_id);
    if(it != map_query_future.end()) {
        if(error_message_string_data_length) {
            std::string error_message((char*)error_message_string_data, error_message_string_data_length);
            //initialize the query for for the result receivement
            _endQueryFutureResult(*it->second, header->field.error, error_message);
        } else {
            //initialize the query for for the result receivement
            _endQueryFutureResult(*it->second, header->field.error);
        }
    }
    return err;
}

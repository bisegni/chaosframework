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

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include <boost/lexical_cast.hpp>

#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common//global.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>

#define IODirectIODriver_LOG_HEAD "[IODirectIODriver] - "

#define IODirectIODriver_LINFO_ INFO_LOG(IODirectIODriver)
#define IODirectIODriver_DLDBG_ DBG_LOG(IODirectIODriver)
#define IODirectIODriver_LERR_ ERR_LOG(IODirectIODriver)

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::utility;

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace chaos_data = chaos::common::data;
namespace chaos_dio = chaos::common::direct_io;
namespace chaos_dio_channel = chaos::common::direct_io::channel;

DEFINE_CLASS_FACTORY(IODirectIODriver, IODataDriver);

//using namespace memcache;
IODirectIODriver::IODirectIODriver(const std::string& alias):
NamedService(alias),
client_instance(NULL),
connectionFeeder(alias, this),
uuid(UUIDUtil::generateUUIDLite()),
shutting_down(false),
evict_dead_url(GlobalConfiguration::getInstance()->getOption<bool>(InitOption::OPT_DATA_DRIVER_REMOVE_DEAD_URL)),
dead_url_timeout(GlobalConfiguration::getInstance()->getOption<int32_t>(InitOption::OPT_DATA_DRIVER_DEAD_URL_TIMEOUT)*1000){}

IODirectIODriver::~IODirectIODriver() {}

void IODirectIODriver::init(void *_init_parameter) throw(CException) {
    shutting_down = false;
    IODataDriver::init(_init_parameter);
    client_instance = NetworkBroker::getInstance()->getSharedDirectIOClientInstance();
    if(!client_instance) throw CException(-1, "Error creating direct io client instance", __PRETTY_FUNCTION__);
}

void IODirectIODriver::deinit() throw(CException) {
    shutting_down = true;
    IODirectIODriver_LINFO_ << "Remove active query";
    //lock all  internal resource that can be effetted by
    ChaosReadLock wmap_loc(map_query_future_mutex);
    
    //scan all remained query
    for(std::map<string, QueryCursor*>::iterator it = map_query_future.begin();
        it != map_query_future.end();
        it++) {
        //erase query curor
        delete(it->second);
    }
    map_query_future.clear();
    
    //remove all url and service
    IODirectIODriver_LINFO_ << "Remove all urls";
    connectionFeeder.clear();
    IODataDriver::deinit();
}

void IODirectIODriver::setEvictionHandler(EvictionUrlHandler _handler) {
    eviction_handler = _handler;
}

const std::string& IODirectIODriver::getUUID() const {
    return uuid;
}

int IODirectIODriver::storeData(const std::string& key,
                                CDWShrdPtr data_to_store,
                                DataServiceNodeDefinitionType::DSStorageType storage_type,
                                const ChaosStringSet& tag_set)  throw(CException) {
    int err = 0;
    CHAOS_ASSERT(data_to_store)
    ChaosReadLock rl(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    SerializationBufferUPtr serialization = data_to_store->getBSONData();
    if(next_client) {
        serialization->disposeOnDelete = false;
        if((err = (int)next_client->device_client_channel->storeAndCacheDataOutputChannel(key,
                                                                                          (void*)serialization->getBufferPtr(),
                                                                                          (uint32_t)serialization->getBufferLen(),
                                                                                          storage_type,
                                                                                          tag_set))) {
            IODirectIODriver_LERR_ << CHAOS_FORMAT("Error storing data into data service %1% with code %2%, data packet size:%3%",%next_client->connection->getServerDescription()%err%serialization->getBufferLen());
            
        }
    } else {
        DEBUG_CODE(IODirectIODriver_DLDBG_ << "No available socket->loose packet, key '"<<key<<"' storage_type:"<<storage_type<<" buffer len:"<<serialization->getBufferLen());
    }
    return err;
}

int IODirectIODriver::storeHealthData(const std::string& key,
                                      CDWShrdPtr data_to_store,
                                      DataServiceNodeDefinitionType::DSStorageType storage_type,
                                      const ChaosStringSet& tag_set) throw(CException) {
    int err = 0;
    CHAOS_ASSERT(data_to_store)
    try{
        ChaosReadLock rl(mutext_feeder);
        IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
        SerializationBufferUPtr serialization = data_to_store->getBSONData();
        if(next_client) {
            serialization->disposeOnDelete = false;
            if((err = (int)next_client->device_client_channel->storeAndCacheHealthData(key,
                                                                                       (void*)serialization->getBufferPtr(),
                                                                                       (uint32_t)serialization->getBufferLen(),
                                                                                       storage_type,
                                                                                       tag_set))) {
                IODirectIODriver_LERR_ << CHAOS_FORMAT("Error storing data into data service %1% with code %2%",%next_client->connection->getServerDescription()%err);
            }
        } else {
            DEBUG_CODE(IODirectIODriver_DLDBG_ << "No available socket->loose packet, key '"<<key<<"' storage_type:"<<storage_type<<" buffer len:"<<serialization->getBufferLen());
        }
    }catch(...){
        IODirectIODriver_LERR_ << "Generic exception error";
        err++;
    }
    return err;
}

char* IODirectIODriver::retriveRawData(const std::string& key, size_t *dim)  throw(CException) {
    char* result = NULL;
    
    boost::shared_lock<boost::shared_mutex> rl(mutext_feeder);
    
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

int IODirectIODriver::retriveMultipleData(const ChaosStringVector& key,
                                          chaos::common::data::VectorCDWShrdPtr& result)  throw(CException) {
    ChaosReadLock rl(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return -1;
    
    int err = (int)next_client->device_client_channel->requestLastOutputData(key,
                                                                             result);
    if(err) {
        IODirectIODriver_LERR_ << "Error retriving data from data service "<<next_client->connection->getServerDescription()<< " with code:" << err;
    }
    return err;
}

int IODirectIODriver::removeData(const std::string& key,
                                 uint64_t start_ts,
                                 uint64_t end_ts) throw(CException) {
    ChaosReadLock rl(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return -1;
    
    int err = (int)next_client->device_client_channel->deleteDataCloud(key,
                                                                       start_ts,
                                                                       end_ts);
    if(err) {
        IODirectIODriver_LERR_ << CHAOS_FORMAT("Error removing data from data service %1% with code %2% for key %3%",%next_client->connection->getServerDescription()%err%key);
    }
    return err;
}

int IODirectIODriver::loadDatasetTypeFromSnapshotTag(const std::string& restore_point_tag_name,
                                                     const std::string& key,
                                                     uint32_t dataset_type,
                                                     chaos_data::CDWShrdPtr& cdw_shrd_ptr) {
    int err = 0;
    ChaosReadLock rl(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    if(!next_client) return 0;
    chaos_dio_channel::DirectIOSystemAPIGetDatasetSnapshotResult snapshot_result;
    if((err = (int)next_client->system_client_channel->getDatasetSnapshotForProducerKey(restore_point_tag_name,
                                                                                        key,
                                                                                        dataset_type,
                                                                                        snapshot_result))) {
        IODirectIODriver_LERR_ << "Error loading the dataset type:"<<dataset_type<< " for key:" << key << " from restor point:" <<restore_point_tag_name;
    } else {
        if(snapshot_result.channel_data) {
            //we have the dataaset
            try {
                cdw_shrd_ptr = snapshot_result.channel_data;
                IODirectIODriver_LINFO_ << "Got dataset type:"<<dataset_type<< " for key:" << key << " from snapshot tag:" <<restore_point_tag_name;
            } catch (std::exception& ex) {
                IODirectIODriver_LERR_ << "Error deserializing the dataset type:"<<dataset_type<< " for key:" << key << " from snapshot tag:" <<restore_point_tag_name << " with error:" << ex.what();
            } catch (...) {
                IODirectIODriver_LERR_ << "Error deserializing the dataset type:"<<dataset_type<< " for key:" << key << " from snapshot tag:" <<restore_point_tag_name;
            }
        }
    }
    return err;
}

void IODirectIODriver::addServerURL(const std::string& url) {
    ChaosWriteLock wl(mutext_feeder);
    if(!common::direct_io::DirectIOClient::checkURL(url)) {
        IODirectIODriver_LERR_ <<  CHAOS_FORMAT("URL %1% is not wel formed", %url);
        return;
    }
    IODirectIODriver_LINFO_ << CHAOS_FORMAT("Add URL %1% removed", %url);
    //add new url to connection feeder
    connectionFeeder.addURL(chaos::common::network::URL(url));
}

void IODirectIODriver::removeServerURL(const std::string& url) {
    ChaosWriteLock wl(mutext_feeder);
    if(!common::direct_io::DirectIOClient::checkURL(url)) {
        IODirectIODriver_LERR_ << CHAOS_FORMAT("URL %1% is not wel formed", %url);
        return;
    }
    IODirectIODriver_LINFO_ << CHAOS_FORMAT("Removing URL %1%", %url);
    //add new url to connection feeder
    connectionFeeder.removeURL(url);
    IODirectIODriver_LINFO_ << CHAOS_FORMAT("URL %1% removed", %url);
}

chaos::common::data::CDataWrapper* IODirectIODriver::updateConfiguration(chaos::common::data::CDataWrapper* newConfigration) {
    //lock the feeder access
    chaos::common::data::CDataWrapper*ret=NULL;
    ChaosWriteLock rl(mutext_feeder);
    //checkif someone has passed us the device indetification
    if(newConfigration->hasKey(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST)){
        chaos_data::CMultiTypeDataArrayWrapperSPtr liveMemAddrConfig = newConfigration->getVectorValue(DataServiceNodeDefinitionKey::DS_DIRECT_IO_FULL_ADDRESS_LIST);
        size_t numerbOfserverAddressConfigured = liveMemAddrConfig->size();
        for ( int idx = 0; idx < numerbOfserverAddressConfigured; idx++ ){
            string serverDesc = liveMemAddrConfig->getStringElementAtIndex(idx);
            if(!common::direct_io::DirectIOClient::checkURL(serverDesc)) {
                IODirectIODriver_DLDBG_ << "Data proxy server description " << serverDesc << " non well formed";
                continue;
            }
            if(connectionFeeder.hasURL(serverDesc)) {
                IODirectIODriver_LERR_ << "Data proxy server description " << serverDesc << " is laredy instaleld in driver";
                continue;
            }
            IODirectIODriver_LINFO_ << CHAOS_FORMAT("Add server %1% to IODirectIODriver", %serverDesc);
            
            //add new url to connection feeder, thi method in case of failure to allocate service will throw an eception
            connectionFeeder.addURL(chaos::common::network::URL(serverDesc));
        }
        if(numerbOfserverAddressConfigured>0){
            ret=newConfigration;
        }
    } else {
        IODirectIODriver_LERR_<<"DS list not present:"<<newConfigration->getJSONString();
    }
    return ret;
}
#pragma mark Query
QueryCursor *IODirectIODriver::performQuery(const std::string& key,
                                            const uint64_t start_ts,
                                            const uint64_t end_ts,
                                            const uint32_t page_len) {
    return performQuery(key,
                        start_ts,
                        end_ts,
                        ChaosStringSet(),
                        page_len);
}

QueryCursor *IODirectIODriver::performQuery(const std::string& key,
                                            const uint64_t start_ts,
                                            const uint64_t end_ts,
                                            const ChaosStringSet& meta_tags,
                                            const uint32_t page_len) {
    QueryCursor *q = new QueryCursor(UUIDUtil::generateUUID(),
                                     connectionFeeder,
                                     key,
                                     start_ts,
                                     end_ts,
                                     meta_tags,
                                     page_len);
    if(q) {
        //add query to map
        ChaosWriteLock wmap_loc(map_query_future_mutex);
        map_query_future.insert(make_pair(q->queryID(), q));
    } else {
        releaseQuery(q);
    }
    return q;
}

QueryCursor *IODirectIODriver::performQuery(const std::string& key,
                                            const uint64_t start_ts,
                                            const uint64_t end_ts,
                                            const uint64_t sequid,
                                            const uint64_t runid,
                                            uint32_t page_len) {
    return performQuery(key,
                        start_ts,
                        end_ts,
                        sequid,
                        runid,
                        ChaosStringSet(),
                        page_len);
}

QueryCursor *IODirectIODriver::performQuery(const std::string& key,
                                            const uint64_t start_ts,
                                            const uint64_t end_ts,
                                            const uint64_t sequid,
                                            const uint64_t runid,
                                            const ChaosStringSet& meta_tags,
                                            uint32_t page_len) {
    QueryCursor *q = new QueryCursor(UUIDUtil::generateUUID(),
                                     connectionFeeder,
                                     key,
                                     start_ts,
                                     end_ts,
                                     sequid,
                                     runid,
                                     meta_tags,
                                     page_len);
    if(q) {
        //add query to map
        ChaosWriteLock wmap_loc(map_query_future_mutex);
        map_query_future.insert(make_pair(q->queryID(), q));
    } else {
        releaseQuery(q);
    }
    return q;
}

void IODirectIODriver::releaseQuery(QueryCursor *query_cursor) {
    //acquire write lock
    if(query_cursor == NULL) return;
    ChaosWriteLock wmap_loc(map_query_future_mutex);
    if(map_query_future.count(query_cursor->queryID())) {
        map_query_future.erase(query_cursor->queryID());
    }
    delete query_cursor;
}
#pragma mark Timer Handler
void IODirectIODriver::timeout() {
    std::set<uint32_t> id_to_remove;
    ChaosStringSet removed_url;
    uint64_t now = TimingUtil::getTimeStamp();
    //scan phase
    LMapUrlEvitionWriteLock evic_wl = map_url_eviction.getWriteLockObject();
    MapUrlEvitionIterator it = map_url_eviction().begin();
    MapUrlEvitionIterator end = map_url_eviction().end();
    while (it != end) {
        if(it->second->evict_ts <= now) {
            id_to_remove.insert(it->second->url_index);
        }
        it++;
    }
    evic_wl->unlock();
    
    //remove fase
    if(evict_dead_url) {
        ChaosWriteLock wl(mutext_feeder);
        for(std::set<uint32_t>::iterator it = id_to_remove.begin(),
            end = id_to_remove.end();
            it != end;
            it++){
            LMapUrlEvitionWriteLock evic_wl = map_url_eviction.getWriteLockObject();
            if(map_url_eviction().count(*it)) {
                //add new url to connection feeder
                const std::string url = map_url_eviction()[*it]->url;
                if(connectionFeeder.hasURL(url)) {
                    removed_url.insert(url);
                    connectionFeeder.removeURL(*it, true);
                }
                map_url_eviction().erase(*it);
            }
        }
        
        if(eviction_handler &&
           id_to_remove.size()) {
            //call handler
            eviction_handler(removed_url);
        }
    }
}

#pragma mark URLServiceFeederHandler hander
void IODirectIODriver::disposeService(void *service_ptr) {
    if(!service_ptr) return;
    IODirectIODriverClientChannels    *next_client = static_cast<IODirectIODriverClientChannels*>(service_ptr);
    //remove me as handler before delete all other this so anymore receive event
    next_client->connection->setEventHandler(NULL);
    if(next_client->system_client_channel) next_client->connection->releaseChannelInstance(next_client->system_client_channel);
    
    if(next_client->device_client_channel) {
        next_client->connection->releaseChannelInstance(next_client->device_client_channel);
    }
    
    client_instance->releaseConnection(next_client->connection);
    delete(next_client);
}

void* IODirectIODriver::serviceForURL(const common::network::URL& url, uint32_t service_index) {
    IODirectIODriver_LINFO_ << "Try to create service for " << url.getURL();
    IODirectIODriverClientChannels * clients_channel = NULL;
    chaos_direct_io::DirectIOClientConnection *tmp_connection = client_instance->getNewConnection(url.getURL());
    if(tmp_connection) {
        clients_channel = new IODirectIODriverClientChannels();
        clients_channel->connection = tmp_connection;
        
        //allocate the client channel
        clients_channel->device_client_channel = (chaos_dio_channel::DirectIODeviceClientChannel*)tmp_connection->getNewChannelInstance("DirectIODeviceClientChannel");
        if(!clients_channel->device_client_channel) {
            IODirectIODriver_LERR_ << "Error creating client device channel for " << url.getURL();
            
            //release conenction
            client_instance->releaseConnection(tmp_connection);
            tmp_connection = NULL;
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
            client_instance->releaseConnection(tmp_connection);
            tmp_connection = NULL;
            //relase struct
            delete(clients_channel);
            clients_channel = NULL;
            return NULL;
        }
        //set this driver instance as event handler for connection
        clients_channel->connection->setEventHandler(this);
        clients_channel->connection->setCustomStringIdentification(boost::lexical_cast<std::string>(service_index));
        IODirectIODriver_LINFO_ << "Connection for " << url.getURL() << " added succesfully";
        return clients_channel;
    } else {
        IODirectIODriver_LERR_ << "Error creating client connection for " << url.getURL();
        return NULL;
    }
}

#pragma mark DirectIOClientConnectionEventHandler
void IODirectIODriver::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
                                   chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
    if(shutting_down) return;
    try {
        LMapUrlEvitionWriteLock evic_wl = map_url_eviction.getWriteLockObject();
        uint32_t service_index = boost::lexical_cast<uint32_t>(client_connection->getCustomStringIdentification());
        switch(event) {
            case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:
                DEBUG_CODE(IODirectIODriver_LINFO_ << "Manage Connected event to service with index " << service_index << " and url" << client_connection->getURL();)
                map_url_eviction().erase(service_index);
                break;
                
            case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:
                if(connectionFeeder.isOnline(service_index)){
                    DEBUG_CODE(IODirectIODriver_LINFO_ << "Manage Disconnected event for service with index " << service_index << " and url" << client_connection->getURL();)
                    connectionFeeder.setURLOffline(service_index);
                    if(map_url_eviction().count(service_index) == 0) {
                        map_url_eviction().insert(MapUrlEvitionPair(service_index, ChaosSharedPtr<OfflineUrlEvicInfo>(new OfflineUrlEvicInfo(service_index,
                                                                                                                                             client_connection->getURL(),
                                                                                                                                             dead_url_timeout))));
                        if(map_url_eviction().size()) {
                            IODirectIODriver_DLDBG_ << "Start time for eviction checking";
                            //we need to turn on the timer
                            async_central::AsyncCentralManager::getInstance()->addTimer(this, 0, GlobalConfiguration::getInstance()->getOption<int32_t>(InitOption::OPT_DATA_DRIVER_EVICT_CHECK_TIME)*1000);
                        }
                    }
                }
                break;
        }
        
    } catch(...){
        IODirectIODriver_LERR_ << "exception handling event identification:" << client_connection->getCustomStringIdentification() << " and url:" << client_connection->getURL();
    }
}

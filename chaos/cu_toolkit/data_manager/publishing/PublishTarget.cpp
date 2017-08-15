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

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/cu_toolkit/data_manager/publishing/PublishTarget.h>

#define INFO    INFO_LOG(PublishTarget)
#define DBG     DBG_LOG(PublishTarget)
#define ERR     ERR_LOG(PublishTarget)

using namespace chaos::common::data;
using namespace chaos::common::data::cache;

using namespace chaos::common::network;

using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

using namespace chaos::cu::data_manager::publishing;

#pragma mark PublishElementAttribute
PublishElementAttribute::PublishElementAttribute():
publishing_mode(chaos::DataServiceNodeDefinitionType::DSStorageTypeUndefined),
publish_rate(0){}

PublishElementAttribute::PublishElementAttribute(const PublishElementAttribute& src):
publishing_mode(src.publishing_mode),
publish_rate(src.publish_rate){}

PublishElementAttribute& PublishElementAttribute::operator=(const PublishElementAttribute& src) {
    publishing_mode = src.publishing_mode;
    publish_rate = src.publish_rate;
    return *this;
}

#pragma mark PublishableElement
PublishableElement::PublishableElement():
dataset_ptr(NULL),
attribute(){}

PublishableElement::PublishableElement(const PublishableElement& _dataset_element):
dataset_ptr(_dataset_element.dataset_ptr),
attribute(_dataset_element.attribute){}

PublishableElement::PublishableElement(const DatasetElement& _dataset_reference,
                                       const PublishElementAttribute& _attribute):
dataset_ptr(&_dataset_reference),
attribute(_attribute){}

#pragma mark PublishTarget Public Methods
PublishTarget::PublishTarget(const std::string &target_name):
connection_feeder(target_name, this) {}

PublishTarget::~PublishTarget() {}

void PublishTarget::clear() {
    connection_feeder.clear(true);
}

bool PublishTarget::addServer(const std::string &server_url_new) {
    //lock the feeder
    ChaosWriteLock wl(mutext_feeder);
    
    if (!DirectIOClient::checkURL(server_url_new)) {
        ERR << CHAOS_FORMAT("Url %1% non well formed", %server_url_new);
        return false;
    }
    //add new url to connection feeder
    connection_feeder.addURL(URL(server_url_new));
    INFO << CHAOS_FORMAT("Added new url %1% into the endpoint", %server_url_new%connection_feeder.getName());
    return true;
}

bool PublishTarget::removeServer(const std::string &server_url_erase) {
    //lock the feeder
    ChaosWriteLock wl(mutext_feeder);
    
    if (!DirectIOClient::checkURL(server_url_erase)) {
        ERR << CHAOS_FORMAT("Url %1% non well formed", %server_url_erase);
        return false;
    }
    //remove server
    connection_feeder.removeURL(connection_feeder.getIndexFromURL(server_url_erase));
    INFO << CHAOS_FORMAT("Remove url %1% from the endpoint", %server_url_erase%connection_feeder.getName());
    return true;
}

void PublishTarget::setAttributeOnDataset(const std::string& dataset_name,
                                          const PublishElementAttribute& publishable_attribute) {
    ChaosWriteLock rl(mutext_map_pub);
    
    if(map_publishable_element.count(dataset_name) == 0) return;
    
    map_publishable_element[dataset_name].attribute = publishable_attribute;
}

void PublishTarget::addDataset(const DatasetElement &publishable_dataset,
                               const PublishElementAttribute& publishable_attribute) {
    ChaosWriteLock rl(mutext_map_pub);
    map_publishable_element.insert(PublishableElementMapPair(publishable_dataset.dataset->getDatasetName(),
                                                             PublishableElement(publishable_dataset,
                                                                                publishable_attribute)));
}

void PublishTarget::removeDataset(const std::string& dataset_name) {
    ChaosWriteLock rl(mutext_map_pub);
    map_publishable_element.erase(dataset_name);
    
}

void PublishTarget::publish() {
    ChaosReadLock rl(mutext_map_pub);
    
    //scan all elements
    for(PublishableElementMapIterator it = map_publishable_element.begin(),
        end = map_publishable_element.end();
        it != end;
        it++) {
        //check the rate for this dataset
        if(publish(it->second) == false) {
            ERR << CHAOS_FORMAT("Error push dataset %1% into the endpoint %2%",
                                %it->first%connection_feeder.getName());
        }
    }
}
#pragma mark PublishTarget Private Methods
bool PublishTarget::publish(const PublishableElement& publishable_element) {
    int err = 0;
    
    //check if dataset has been changed
    if (publishable_element.dataset_ptr->dataset_value_cache.hasChanged())
        return false;
    
    //lock feeder
    ChaosReadLock rl(mutext_feeder);
    
    ChaosUniquePtr<SerializationBuffer> serialization(getDataPack(publishable_element.dataset_ptr->dataset_value_cache)->getBSONData());
    
    //get next available client
    PublishTargetClientChannel *next_client = static_cast<PublishTargetClientChannel *>(connection_feeder.getService());
    serialization->disposeOnDelete = !next_client;
    if (next_client) {
        //free the packet
        serialization->disposeOnDelete = false;
        if ((err = (int)next_client->device_client_channel->storeAndCacheDataOutputChannel(publishable_element.dataset_ptr->dataset->getDatasetKey(),
                                                                                           (void *)serialization->getBufferPtr(),
                                                                                           (uint32_t)serialization->getBufferLen(),
                                                                                           publishable_element.attribute.publishing_mode))) {
            ERR << CHAOS_FORMAT("Error storing data into data service %1% with code: %2%",
                                %next_client->connection->getServerDescription()%err);
        }
    } else {
        DEBUG_CODE(DBG << "No available socket for publish data");
    }
    return true;
}

ChaosUniquePtr<chaos::common::data::CDataWrapper> PublishTarget::getDataPack(const chaos::common::data::cache::AttributeCache &attribute_cache) {
    //clock the cache
    ReadSharedCacheLockDomain read_lock_on_cache(attribute_cache.mutex);
    
    ChaosUniquePtr<chaos::common::data::CDataWrapper> data_pack(new CDataWrapper());
    
    //write timestamp
    attribute_cache.exportToCDataWrapper(*data_pack);
    
    return data_pack;
}

void PublishTarget::disposeService(void *service_ptr) {
    CHAOS_ASSERT(service_ptr);
    
    PublishTargetClientChannel *next_client = static_cast<PublishTargetClientChannel *>(service_ptr);
    if (next_client->device_client_channel)
        next_client->connection->releaseChannelInstance(next_client->device_client_channel);
    NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->releaseConnection(next_client->connection);
    delete (next_client);
}

void *PublishTarget::serviceForURL(const URL &url, uint32_t service_index) {
    INFO << "try to add connection for " << url.getURL();
    ChaosUniquePtr<PublishTargetClientChannel> endpoint_connection(new PublishTargetClientChannel());
    endpoint_connection->connection =
    NetworkBroker::getInstance()->getSharedDirectIOClientInstance()->getNewConnection(url.getURL());
    if (endpoint_connection->connection) {
        
        
        //allocate the client channel
        endpoint_connection->device_client_channel =
        static_cast<DirectIODeviceClientChannel *>(endpoint_connection->connection->getNewChannelInstance("DirectIODeviceClientChannel"));
        if (endpoint_connection->device_client_channel == NULL) {
            ERR << "Error creating client device channel for " << url.getURL();
            
            //release conenction
            NetworkBroker::getInstance()->getSharedDirectIOClientInstance()
            ->releaseConnection(endpoint_connection->connection);
            return NULL;
        }
        
        //set this class instance as event handler for connection
        endpoint_connection->connection->setEventHandler(this);
        endpoint_connection->connection->setCustomStringIdentification(boost::lexical_cast<std::string>(service_index));
    } else {
        ERR << "Error creating client connection for " << url.getURL();
    }
    INFO << CHAOS_FORMAT("connection for %1% added succesfully", %url.getURL());
    return endpoint_connection.release();
}

void PublishTarget::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
                                chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
    //if the channel has bee disconnected turn the relative index offline, if onli reput it online
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    uint32_t service_index = boost::lexical_cast<uint32_t>(client_connection->getCustomStringIdentification());
    switch (event) {
        case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:DEBUG_CODE(INFO <<
                                                                                                                   CHAOS_FORMAT("Manage Connected event to service with index %1% and url %2%",
                                                                                                                                % service_index
                                                                                                                                % client_connection
                                                                                                                                ->getURL());)
            connection_feeder.setURLOnline(service_index);
            break;
            
        case chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:DEBUG_CODE(INFO <<
                                                                                                                      CHAOS_FORMAT("Manage Disconnected event to service with index %1% and url %2%",
                                                                                                                                   % service_index
                                                                                                                                   % client_connection
                                                                                                                                   ->getURL());)
            connection_feeder.setURLOffline(service_index);
            break;
    }
}

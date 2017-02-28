/*
 *	ZMQDirectIOClinet.cpp
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
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>
#include <chaos/common/data/cache/FastHash.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <string.h>
#include <assert.h>     /* assert */

#define ZMQDIO_LOG_HEAD "["<<getName()<<"] - "

#define ZMQDIOLAPP_ LAPP_ << ZMQDIO_LOG_HEAD
#define ZMQDIOLDBG_ LDBG_ << ZMQDIO_LOG_HEAD << __FUNCTION__ << " - "
#define ZMQDIOLERR_ LERR_ << ZMQDIO_LOG_HEAD

using namespace chaos::common::utility;

using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::impl;

typedef boost::unique_lock<boost::shared_mutex>	ZMQDirectIOClientWriteLock;
typedef boost::shared_lock<boost::shared_mutex> ZMQDirectIOClientReadLock;

DEFINE_CLASS_FACTORY(ZMQDirectIOClient, DirectIOClient);

//------------------------------STATIC METHOD---------------------------------
int ZMQDirectIOClient::readMesg(void* s, zmq_event_t* event, char* ep) {
    int rc ;
    zmq_msg_t msg1;  // binary part
    zmq_msg_init (&msg1);
    zmq_msg_t msg2;  //  address part
    zmq_msg_init (&msg2);
    rc = zmq_msg_recv (&msg1, s, 0);
    if (rc == -1)
        return 1 ;
    if(zmq_msg_more(&msg1) == 0) return 1;
    rc = zmq_msg_recv (&msg2, s, 0);
    if (rc == -1)
        return 1;
    if(zmq_msg_more(&msg2) != 0) return 1;
    // copy binary data to event struct
    const char* data = (char*)zmq_msg_data(&msg1);
    memcpy(&(event->event), data, sizeof(event->event));
    memcpy(&(event->value), data+sizeof(event->event), sizeof(event->value));
    // copy address part
    const size_t len = zmq_msg_size(&msg2) ;
    ep = (char*)memcpy(ep, zmq_msg_data(&msg2), len);
    *(ep + len) = 0 ;
    return 0 ;
}

void *ZMQDirectIOClient::socketMonitor (void *ctx, const char * address, ConnectionMonitorInfo *monitor_info) {
    const int linger_period = 0;
    std::string monitor_inproc_address = address;
    zmq_event_t event;
    static char addr[1025];
    int rc;
    DirectIOClientConnection *connection = NULL;
    //std::vector<std::string> server_desc_tokens;
    DEBUG_CODE(ZMQDIOLDBG_ << "Start monitor for " << monitor_inproc_address;)
    
    monitor_info->monitor_socket = zmq_socket (ctx, ZMQ_PAIR);
    if(monitor_info->monitor_socket == NULL) return NULL;
    rc = zmq_setsockopt (monitor_info->monitor_socket, ZMQ_LINGER, &linger_period, sizeof(int));
    
    rc = zmq_connect (monitor_info->monitor_socket, address);
    if(rc) return NULL;
    while (monitor_info->run && !readMesg(monitor_info->monitor_socket, &event, addr)) {
        if((connection = map_connections.accessItem(monitor_info->unique_identification))) {
            switch (event.event) {
                case ZMQ_EVENT_CONNECTED:
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECTED to " << connection->getServerDescription();)
                    forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected);
                    break;
                case ZMQ_EVENT_CONNECT_DELAYED:
                    //forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected);
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECT_DELAYED " << connection->getServerDescription();)
                    break;
                case ZMQ_EVENT_CONNECT_RETRIED:
                    //forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECT_RETRIED " << connection->getServerDescription();)
                    break;
                case ZMQ_EVENT_CLOSE_FAILED:
                    forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_CLOSE_FAILED " << connection->getServerDescription();)
                    break;
                case ZMQ_EVENT_CLOSED:
                    forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_CLOSED " << connection->getServerDescription();)
                    break;
                case ZMQ_EVENT_DISCONNECTED:
                    forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_DISCONNECTED " << connection->getServerDescription();)
                    break;
                case ZMQ_EVENT_MONITOR_STOPPED:
                    monitor_info->run = false;
                    DEBUG_CODE(ZMQDIOLDBG_ << "ZMQ_EVENT_MONITOR_STOPPED " << connection->getServerDescription();)
                    break;
            }
        }
    }
    zmq_close(monitor_info->monitor_socket);
    DEBUG_CODE(ZMQDIOLDBG_ << "End monitor for " << monitor_inproc_address;)
    return NULL;
}

//------------------------------STATIC METHOD---------------------------------


ZMQDirectIOClient::ZMQDirectIOClient(std::string alias):
DirectIOClient(alias),
priority_port(0),
service_port(0),
thread_run(false),
zmq_context(NULL){};

ZMQDirectIOClient::~ZMQDirectIOClient(){};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    int err = 0;
    MapZMQConfiguration default_configuration;
    default_configuration["ZMQ_IO_THREADS"] = "1";
    
    DirectIOClient::init(init_data);
    ZMQDIOLAPP_ << "Allocating zmq context";
    thread_run= true;
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    if((err = ZMQBaseClass::configureContextWithStartupParameter(zmq_context,
                                                                 default_configuration,
                                                                 chaos::GlobalConfiguration::getInstance()->getDirectIOClientImplKVParam(),
                                                                 "ZMQ DirectIO Client"))) {
        throw chaos::CException(2, "Error configuring service socket", __FUNCTION__);
    }
    
    
    ZMQDIOLAPP_ << "Inizilizing zmq implementation with zmq lib version = " << ZMQ_VERSION;
    ZMQDIOLAPP_ << "Set number of thread for the contex";
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    
    ZMQDIOLAPP_ << "Initialized";
}

//! Deinit the implementation
void ZMQDirectIOClient::deinit() throw(chaos::CException) {
    int err = 0;
    //remove all active connection (never need to be exists at this step)
    map_connections.clearElement();
    //destroy the zmq context
    ZMQDIOLAPP_ << "Destroing zmq context";
    thread_run = false;
    err = zmq_ctx_destroy(zmq_context);
    if(err) ZMQDIOLERR_ << "Error closing context";
    //monitor_thread_group.join_all();
    
    zmq_context = NULL;
    ZMQDIOLAPP_ << "ZMQ context destroyed";
    DirectIOClient::deinit();
}

DirectIOClientConnection *ZMQDirectIOClient::_getNewConnectionImpl(std::string server_description,
                                                                   uint16_t endpoint) {
    //allocate client
    ZMQDirectIOClientConnection *connection = new ZMQDirectIOClientConnection(zmq_context,
                                                                              server_description,
                                                                              endpoint);
    if(connection == NULL) return NULL;
    try{
        InizializableService::initImplementation(connection, NULL, "ZMQDirectIOClientConnection", __PRETTY_FUNCTION__);
        //register client with the hash of the xzmq decoded endpoint address (tcp://ip:port)
        DEBUG_CODE(ZMQDIOLDBG_ << "Register client for " << server_description << " with zmq decoded hash " << connection->getUniqueUUID();)
        map_connections.registerElement(connection->getUniqueUUID(), connection);
    } catch (...) {}
    return connection;
}

void ZMQDirectIOClient::_releaseConnectionImpl(DirectIOClientConnection *connection_to_release) {
    ZMQDirectIOClientConnection *conn=reinterpret_cast<ZMQDirectIOClientConnection*>(connection_to_release);
    if(!conn) return;
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(conn, "ZMQDirectIOClientConnection", __PRETTY_FUNCTION__););
    //CHAOS_ASSERT(conn->monitor_info)
    //stop the monitor
    ZMQDIOLAPP_ << "Release the connection for: " << connection_to_release->getServerDescription();
    map_connections.deregisterElementKey(conn->getUniqueUUID());
    delete(connection_to_release);
    
}


void ZMQDirectIOClient::freeObject(const DCKeyObjectContainer::TKOCElement& element) {
    if(!element.element) return;
    DirectIOClientConnection *connection = element.element;
    ZMQDIOLAPP_ << "Autorelease connection for " << connection->getServerDescription();
    releaseConnection(connection);
}

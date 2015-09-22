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
        if((connection = DCKeyObjectContainer::accessItem(monitor_info->unique_identification))) {
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


ZMQDirectIOClient::ZMQDirectIOClient(std::string alias):DirectIOClient(alias){
    thread_run = false;
    zmq_context = NULL;
};

ZMQDirectIOClient::~ZMQDirectIOClient(){
};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    DirectIOClient::init(init_data);
    ZMQDIOLAPP_ << "Allocating zmq context";
    thread_run= true;
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    
    ZMQDIOLAPP_ << "Inizilizing zmq implementation with zmq lib version = " << ZMQ_VERSION;
    ZMQDIOLAPP_ << "Set number of thread for the contex";
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    
    ZMQDIOLAPP_ << "Initialized";
}

//! Deinit the implementation
void ZMQDirectIOClient::deinit() throw(chaos::CException) {
    int err = 0;
    //remove all active connection (never need to be exists at this step)
    DCKeyObjectContainer::clearElement();
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

DirectIOClientConnection *ZMQDirectIOClient::_getNewConnectionImpl(std::string server_description, uint16_t endpoint) {
    int err = 0;
    const int output_buffer_dim = 10;
    const int linger_period = 0;
    const int timeout = 5000;
    const int min_reconnection_ivl = 5000;
    const int max_reconnection_ivl = 10000;
    
    void *socket_priority = NULL;
    void *socket_service = NULL;
    
    std::string url;
    std::string error_str;
    std::string priority_endpoint;
    std::string service_endpoint;
    
    std::vector<std::string> resolved_ip;
    
    ZMQDirectIOClientConnection *result = NULL;
    
    try {
        
        DEBUG_CODE(ZMQDIOLDBG_ << "Allocating priority socket";)
        socket_priority = zmq_socket (zmq_context, ZMQ_DEALER);
        if(socket_priority == NULL) throw chaos::CException(1, "Error creating priority socket", __FUNCTION__);
        
        //---------------------------------------------------------------------------------------------------------------
        err = zmq_setsockopt (socket_priority, ZMQ_LINGER, &linger_period, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting linger on priority socket option", __FUNCTION__);
        //set output queue dime
        err = zmq_setsockopt (socket_priority, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on priority socket option", __FUNCTION__);
        
        //set input queue dime
        err = zmq_setsockopt (socket_priority, ZMQ_RCVHWM, &output_buffer_dim, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on priority socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_priority, ZMQ_RCVTIMEO, &timeout, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_RCVTIMEO on priority socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_priority, ZMQ_SNDTIMEO, &timeout, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_SNDTIMEO on priority socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_priority, ZMQ_RECONNECT_IVL, &min_reconnection_ivl, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on priority socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_priority, ZMQ_RECONNECT_IVL_MAX, &max_reconnection_ivl, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on priority socket option", __FUNCTION__);
        
        //---------------------------------------------------------------------------------------------------------------
        DEBUG_CODE(ZMQDIOLDBG_ << "Allocating service socket";)
        socket_service = zmq_socket (zmq_context, ZMQ_DEALER);
        if(socket_service == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
        
        err = zmq_setsockopt (socket_service, ZMQ_LINGER, &linger_period, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting linger on service socket option", __FUNCTION__);
        //set output queue dime
        err = zmq_setsockopt (socket_service, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on service socket option", __FUNCTION__);
        
        //set input queue dime
        err = zmq_setsockopt (socket_service, ZMQ_RCVHWM, &output_buffer_dim, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on service socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_service, ZMQ_RCVTIMEO, &timeout, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_RCVTIMEO on service socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_service, ZMQ_SNDTIMEO, &timeout, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_SNDTIMEO on service socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_service, ZMQ_RECONNECT_IVL, &min_reconnection_ivl, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on service socket option", __FUNCTION__);
        
        err = zmq_setsockopt (socket_service, ZMQ_RECONNECT_IVL_MAX, &max_reconnection_ivl, sizeof(int));
        if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on service socket option", __FUNCTION__);
        //---------------------------------------------------------------------------------------------------------------
        //allocate client
        result = new ZMQDirectIOClientConnection(server_description, socket_priority, socket_service, endpoint);
        err = setAndReturnID(socket_priority, result->priority_identity);
        if(err) throw chaos::CException(err, "Error setting identity on priority socket", __FUNCTION__);
        
        err = setAndReturnID(socket_service, result->service_identity);
        if(err) throw chaos::CException(err, "Error setting identity on service socket", __FUNCTION__);
        
        //set the server information on socket
        decoupleServerDescription(server_description, priority_endpoint, service_endpoint);
        
        
        //DEBUG_CODE(ZMQDIOLDBG_ << "Allocating monitor socket thread for monitor url " << monitor_url;)
        result->monitor_info.run = true;
        result->monitor_info.monitor_thread = NULL;
        result->monitor_info.monitor_socket = NULL;
        result->monitor_info.unique_identification = result->getUniqueUUID();
        //result->getConnectionHash();
        
        //register socket for monitoring
        result->monitor_info.monitor_url = boost::str( boost::format("inproc://%1%") % result->getUniqueUUID());
        err = zmq_socket_monitor(socket_priority, result->monitor_info.monitor_url.c_str(), ZMQ_EVENT_ALL);
        if(err) throw chaos::CException(err, "Error activating monitor on service socket", __FUNCTION__);
        
        result->monitor_info.monitor_thread = new boost::thread(boost::bind(&ZMQDirectIOClient::socketMonitor, this, zmq_context, result->monitor_info.monitor_url.c_str(), &result->monitor_info));
        
        //register client with the hash of the xzmq decoded endpoint address (tcp://ip:port)
        DEBUG_CODE(ZMQDIOLDBG_ << "Register client for " << server_description << " with zmq decoded hash " << result->getUniqueUUID();)
        DCKeyObjectContainer::registerElement(result->getUniqueUUID(), result);
        
        url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
        DEBUG_CODE(ZMQDIOLDBG_ << "connect to priority endpoint " << url;)
        err = zmq_connect(socket_priority, url.c_str());
        if(err) {
            error_str = boost::str( boost::format("Error connecting priority socket to %1%") % priority_endpoint);
            ZMQDIOLERR_ << error_str;
            throw chaos::CException(err, error_str, __FUNCTION__);
        }
        
        //add monitor on priority socket
        url = boost::str( boost::format("tcp://%1%") % service_endpoint);
        DEBUG_CODE(ZMQDIOLDBG_ << "connect to service endpoint " << url;)
        err = zmq_connect(socket_service, url.c_str());
        if(err) {
            error_str = boost::str( boost::format("Error connecting service socket to %1%") % service_endpoint);
            ZMQDIOLERR_ << error_str;
            throw chaos::CException(err, error_str, __FUNCTION__);
        }
    } catch(chaos::CException& ex) {
        
        if(socket_priority) {
            err = zmq_close(socket_priority);
            if(err) ZMQDIOLERR_ << "Error closing service socket";
        }
        if(socket_service) {
            err = zmq_close(socket_service);
            if(err) ZMQDIOLERR_ << "Error closing service socket";
        }
        if(result) {
            DCKeyObjectContainer::deregisterElementKey(result->getUniqueUUID());
            delete(result);
        }
    }
    return result;
}

void ZMQDirectIOClient::_releaseConnectionImpl(DirectIOClientConnection *connection_to_release) {
    int err = 0;
    ZMQDirectIOClientConnection *conn=reinterpret_cast<ZMQDirectIOClientConnection*>(connection_to_release);
    if(!conn) return;
    //CHAOS_ASSERT(conn->monitor_info)
    //stop the monitor
    ZMQDIOLAPP_ << "Release the connection for: " << connection_to_release->getServerDescription();
    
    //disable monitor
    conn->monitor_info.run = false;
    //err = zmq_disconnect(conn->socket_priority, "tcp://0.0.0.0.:1111");
    err = zmq_socket_monitor(conn->socket_priority, NULL, 0);
    if(err) ZMQDIOLERR_ << "Error closing monitor socket for " << conn->getServerDescription();
    
    if(conn->monitor_info.monitor_thread) {
        if(conn->monitor_info.monitor_thread->joinable())
            conn->monitor_info.monitor_thread->join();
        delete(conn->monitor_info.monitor_thread);
    }
    ZMQDIOLAPP_ << "Disabled monitor socket for " << conn->getServerDescription();
    
    err = zmq_close(conn->socket_priority);
    if(err) ZMQDIOLERR_ << "Error disconnecting priority socket for " << conn->getServerDescription();
    //seem that disconnection from somewhere can let the monitor will repsond to the disable action
    
    ZMQDIOLAPP_ << "Close service socket for" << conn->getServerDescription();
    err = zmq_close(conn->socket_service);
    if(err) ZMQDIOLERR_ << "Error closing service socket for " << conn->getServerDescription();
    
    //stop the monitor
    //	if(conn->monitor_info &&
    //       conn->monitor_info->monitor_socket) {
    
    //	}
    
    DCKeyObjectContainer::deregisterElementKey(conn->getUniqueUUID());
    delete(connection_to_release);
    
}


void ZMQDirectIOClient::freeObject(uint32_t hash, DirectIOClientConnection *connection) {
    if(!connection) return;
    ZMQDIOLAPP_ << "Autorelease connection for " << connection->getServerDescription();
    releaseConnection(connection);
}

/*
 *	ZMQDirectIOClinet.cpp
 *	!CHOAS
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
#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>
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


//------------------------------STATIC METHOD---------------------------------
int ZMQDirectIOClient::readMesg(void* s, zmq_event_t* event, char* ep) {
    int rc ;
    zmq_msg_t msg1;  // binary part
    zmq_msg_init (&msg1);
    zmq_msg_t msg2;  //  address part
    zmq_msg_init (&msg2);
    rc = zmq_msg_recv (&msg1, s, 0);
    if (rc == -1 && zmq_errno() == ETERM)
		return 1 ;
    assert (rc != -1);
    assert (zmq_msg_more(&msg1) != 0);
    rc = zmq_msg_recv (&msg2, s, 0);
    if (rc == -1 && zmq_errno() == ETERM)
		return 1;
    assert (rc != -1);
    assert (zmq_msg_more(&msg2) == 0);
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

void *ZMQDirectIOClient::socketMonitor (void *ctx, const char * address) {
    zmq_event_t event;
    static char addr[1025];
    int rc;
	uint32_t server_addr_hash;
	DirectIOClientConnection *connection = NULL;
	std::vector<std::string> server_desc_tokens;
    ZMQDIOLDBG_ << "starting monitor";
	
    void *s = zmq_socket (ctx, ZMQ_PAIR);
    assert (s);
	
    rc = zmq_connect (s, address);
    if(rc) return NULL;
    while (!readMesg(s, &event, addr)) {
		//server_desc_tokens.clear();
		//boost::algorithm::split(server_desc_tokens, addr, boost::algorithm::is_any_of("-"), boost::algorithm::token_compress_on);
		//if(server_desc_tokens.size() != 2) continue;
		server_addr_hash = chaos::common::data::cache::FastHash::hash(addr, std::strlen(addr), 0);
		if((connection = TemplatedKeyObjectContainer::accessItem(server_addr_hash))) {
			switch (event.event) {
				case ZMQ_EVENT_CONNECTED:
					ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECTED " << event.value << "-" << addr;
					forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected);
					break;
				case ZMQ_EVENT_CONNECT_DELAYED:
					forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
					ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECT_DELAYED " << event.value << "-" << addr;
					break;
				case ZMQ_EVENT_CONNECT_RETRIED:
					forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
					ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECT_RETRIED " << event.value << "-" << addr;
					break;
				case ZMQ_EVENT_CLOSE_FAILED:
					ZMQDIOLDBG_ << "ZMQ_EVENT_CLOSE_FAILED " << event.value << "-" << addr;
					break;
				case ZMQ_EVENT_CLOSED:
					forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
					ZMQDIOLDBG_ << "ZMQ_EVENT_CLOSED " << event.value << "-" << addr;
					break;
				case ZMQ_EVENT_DISCONNECTED:
					forwardEventToClientConnection(connection , DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected);
					ZMQDIOLDBG_ << "ZMQ_EVENT_DISCONNECTED " << event.value << "-" << addr;
					break;
			}
			
		}
    }
    zmq_close (s);
    return NULL;
}

//------------------------------STATIC METHOD---------------------------------


ZMQDirectIOClient::ZMQDirectIOClient(string alias):DirectIOClient(alias){
	thread_run = false;
	zmq_context = NULL;
};

ZMQDirectIOClient::~ZMQDirectIOClient(){
};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
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
    //destroy the zmq context
    ZMQDIOLAPP_ << "Destroing zmq context";
	thread_run = false;
	err = zmq_ctx_destroy(zmq_context);
    if(err) ZMQDIOLERR_ << "Error closing context";
	monitor_thread_group.join_all();
	
    zmq_context = NULL;
	ZMQDIOLAPP_ << "ZMQ context destroyed";
    DirectIOClient::deinit();
}

DirectIOClientConnection *ZMQDirectIOClient::getNewConnection(std::string server_description) {
	int err = 0;
	const int output_buffer_dim = 1;
	const int linger_period = 0;
	const int min_reconnection_ivl = 500;
	const int max_reconnection_ivl = 5000;
	
	void *socket_priority = NULL;
	void *socket_service = NULL;
	
	std::string url;
	std::string error_str;
	std::string priority_endpoint;
	std::string service_endpoint;
	std::string priority_identity = UUIDUtil::generateUUIDLite();
    std::string service_identity = UUIDUtil::generateUUIDLite();
	
	std::vector<std::string> resolved_ip;
	
	ZMQDirectIOClientConnection *result = NULL;
	
	try {
		ZMQDIOLAPP_ << "Allocating priority socket";
		socket_priority = zmq_socket (zmq_context, ZMQ_DEALER);
		if(socket_priority == NULL) throw chaos::CException(1, "Error creating priority socket", __FUNCTION__);
		
		//---------------------------------------------------------------------------------------------------------------
		err = zmq_setsockopt (socket_priority, ZMQ_LINGER, &linger_period, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting linger on priority socket option", __FUNCTION__);
		//set output queue dime
		err = zmq_setsockopt (socket_priority, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on priority socket option", __FUNCTION__);
		
		err = zmq_setsockopt (socket_priority, ZMQ_RECONNECT_IVL, &min_reconnection_ivl, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on priority socket option", __FUNCTION__);
		
		err = zmq_setsockopt (socket_priority, ZMQ_RECONNECT_IVL_MAX, &max_reconnection_ivl, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on priority socket option", __FUNCTION__);

		//---------------------------------------------------------------------------------------------------------------
		ZMQDIOLAPP_ << "Allocating service socket";
		socket_service = zmq_socket (zmq_context, ZMQ_DEALER);
		if(socket_service == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
		
		err = zmq_setsockopt (socket_service, ZMQ_LINGER, &linger_period, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting linger on service socket option", __FUNCTION__);
		//set output queue dime
		err = zmq_setsockopt (socket_service, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on service socket option", __FUNCTION__);
		
		err = zmq_setsockopt (socket_service, ZMQ_RECONNECT_IVL, &min_reconnection_ivl, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on priority socket option", __FUNCTION__);
		
		err = zmq_setsockopt (socket_service, ZMQ_RECONNECT_IVL_MAX, &max_reconnection_ivl, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_RECONNECT_IVL on priority socket option", __FUNCTION__);

		//---------------------------------------------------------------------------------------------------------------
		//allocate client
		result = new ZMQDirectIOClientConnection(server_description, socket_priority, socket_service);
		
		//set the server information on socket
		ServerFeeder::decoupleServerDescription(server_description, priority_endpoint, service_endpoint);
		
		std::string monitor_url = boost::str( boost::format("inproc://%1%") % priority_endpoint);
		err = zmq_socket_monitor(socket_priority, monitor_url.c_str(), ZMQ_EVENT_ALL);
		if(err) throw chaos::CException(err, "Error activating monitor on service socket", __FUNCTION__);
		
		ZMQDIOLAPP_ << "Allocating monitor socket thread for monitor url " << monitor_url;
		monitor_thread_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOClient::socketMonitor, this, zmq_context, monitor_url.c_str())));
		
		url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
		//infer the zmq endpoint address
		std::string temp_priority_endpoint = url;
		size_t end_pos = temp_priority_endpoint.rfind(":") - 6;
		std::string hostname = temp_priority_endpoint.substr(6, end_pos);
		std::string port = temp_priority_endpoint.substr(temp_priority_endpoint.rfind(":")+1);
		chaos::InetUtility::queryDns(hostname, resolved_ip);
		if(resolved_ip.size()) {
			temp_priority_endpoint = boost::str( boost::format("tcp://%1%:%2%") % resolved_ip[0] % port);
		}
		result->zmq_addr_hash = chaos::common::data::cache::FastHash::hash(temp_priority_endpoint.c_str(), temp_priority_endpoint.size(), 0);
		
		//register client with the hash of the xzmq decoded endpoint address (tcp://ip:port)
		ZMQDIOLDBG_ << "Register client for " << server_description << " with zmq decoded hash " << result->zmq_addr_hash;
		TemplatedKeyObjectContainer::registerElement(result->zmq_addr_hash, result);
		
		ZMQDIOLDBG_ << "connect to priority endpoint " << url;
		err = zmq_connect(socket_priority, url.c_str());
		if(err) {
			error_str = boost::str( boost::format("Error connecting priority socket to %1%") % priority_endpoint);
			ZMQDIOLERR_ << error_str;
			throw chaos::CException(err, error_str, __FUNCTION__);
		}
		
		//add monitor on priority socket
		url = boost::str( boost::format("tcp://%1%") % service_endpoint);
		ZMQDIOLDBG_ << "connect to service endpoint " << url;
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
			TemplatedKeyObjectContainer::deregisterElementKey(result->zmq_addr_hash);
			delete(result);
		}
	}
    return result;
}

void ZMQDirectIOClient::releaseConnection(DirectIOClientConnection *connection_to_release) {
	ZMQDIOLAPP_ << "Dispose the connection";
	int err = 0;
	ZMQDirectIOClientConnection *conn=reinterpret_cast<ZMQDirectIOClientConnection*>(connection_to_release);
	if(!conn) return;
	
    err = zmq_close(conn->socket_priority);
    if(err) ZMQDIOLERR_ << "Error closing priority socket";
    //zmq_close(socket_priority);
    
    ZMQDIOLAPP_ << "Close service socket";
    err = zmq_close(conn->socket_service);
    if(err) ZMQDIOLERR_ << "Error closing service socket";
	TemplatedKeyObjectContainer::deregisterElementKey(conn->zmq_addr_hash);
	
	delete(connection_to_release);
	
}


void ZMQDirectIOClient::freeObject(uint32_t hash, DirectIOClientConnection *connection) {
	releaseConnection(connection);
}

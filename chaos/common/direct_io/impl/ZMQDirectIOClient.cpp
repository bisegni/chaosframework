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
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>

#include <chaos/common/data/cache/FastHash.h>

#include <boost/format.hpp>

#include <zmq.h>
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

ZMQDirectIOClient::ZMQDirectIOClient(string alias):DirectIOClient(alias){
	thread_run = false;
	zmq_context = NULL;
	socket_monitor = NULL;
};

ZMQDirectIOClient::~ZMQDirectIOClient(){
};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    ZMQDIOLAPP_ << "Allocating zmq context";
	
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

    zmq_context = NULL;
	ZMQDIOLAPP_ << "ZMQ context destroyed";
    DirectIOClient::deinit();
}




DirectIOClientConnection *ZMQDirectIOClient::getNewConnection(std::string server_description) {
	
	int err = 0;
	int output_buffer_dim = 1;
    int linger_period = 0;
	
	void *socket_priority = NULL;
	void *socket_service = NULL;
	
	std::string url;
	std::string error_str;
	std::string priority_endpoint;
	std::string service_endpoint;
	std::string priority_identity = UUIDUtil::generateUUIDLite();
    std::string service_identity = UUIDUtil::generateUUIDLite();
	
	ZMQDirectIOClientConnection *result = NULL;
	
	try {
		ZMQDIOLAPP_ << "Allocating priority socket";
		socket_priority = zmq_socket (zmq_context, ZMQ_DEALER);
		if(socket_priority == NULL) throw chaos::CException(1, "Error creating priority socket", __FUNCTION__);
		
		err = zmq_setsockopt (socket_priority, ZMQ_LINGER, &linger_period, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting linger on priority socket option", __FUNCTION__);
		//set output queue dime
		err = zmq_setsockopt (socket_priority, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on priority socket option", __FUNCTION__);
		
		ZMQDIOLAPP_ << "Allocating service socket";
		socket_service = zmq_socket (zmq_context, ZMQ_DEALER);
		if(socket_service == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
		
		err = zmq_setsockopt (socket_service, ZMQ_LINGER, &linger_period, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting linger on service socket option", __FUNCTION__);
		//set output queue dime
		err = zmq_setsockopt (socket_service, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
		if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on service socket option", __FUNCTION__);
				
		/*err = zmq_socket_monitor(socket_priority, "inproc://monitor.req", ZMQ_EVENT_ALL);
		if(err) throw chaos::CException(err, "Error activating monitor on service socket", __FUNCTION__);
		
		if(!socket_monitor) {
			ZMQDIOLAPP_ << "Allocating monitor socket";
			socket_monitor = zmq_socket (zmq_context, ZMQ_PAIR);
			if(err) throw chaos::CException(err, "Error creating monitor socket", __FUNCTION__);
			err = zmq_connect(socket_monitor, "inproc://monitor.req");
			if(err) throw chaos::CException(err, "Error connecting monitor socket", __FUNCTION__);
			thread_run= true;
			monitor_thread.reset(new boost::thread(boost::bind(&ZMQDirectIOClient::monitorWorker, this)));
		}*/
		
		//set the server information on socket
		ServerFeeder::decoupleServerDescription(server_description, priority_endpoint, service_endpoint);
		
		url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
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
		
		//allocate client
		result = new ZMQDirectIOClientConnection(server_description, socket_priority, socket_service);
		TemplatedKeyObjectContainer::registerElement(chaos::common::data::cache::FastHash::hash(server_description.c_str(), server_description.size(), 0), result);
	} catch(chaos::CException& ex) {
		if(socket_priority) {
			err = zmq_close(socket_priority);
			if(err) ZMQDIOLERR_ << "Error closing service socket";
		}
		if(socket_service) {
			err = zmq_close(socket_service);
			if(err) ZMQDIOLERR_ << "Error closing service socket";
		}
		if(result) delete(result);
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
	
	delete(connection_to_release);
	
}

//! check the connection with the endpoint for the two socket
void *ZMQDirectIOClient::monitorWorker() {
	zmq_event_t event;
    int err = 0;
	ZMQDIOLAPP_ << "Entering monitor loop";
    while (thread_run) {
		zmq_msg_t msg;
        zmq_msg_init (&msg);
        err = zmq_recvmsg (socket_monitor, &msg, 0);
        if (err == -1 && zmq_errno() == ETERM) break;
        assert (err != -1);
        memcpy (&event, zmq_msg_data (&msg), sizeof (event));
        switch (event.event) {
			case ZMQ_EVENT_CONNECTED:
				ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECTED:"<<event.data.connected.addr;
				break;
			case ZMQ_EVENT_CONNECT_DELAYED:
				ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECT_DELAYED:"<<event.data.connect_delayed.addr;
				break;
			case ZMQ_EVENT_CONNECT_RETRIED:
				ZMQDIOLDBG_ << "ZMQ_EVENT_CONNECT_RETRIED:"<<event.data.connect_retried.addr;
				break;
			case ZMQ_EVENT_CLOSED:
				ZMQDIOLDBG_ << "ZMQ_EVENT_CLOSED:"<<event.data.closed.addr;;
				break;
			case ZMQ_EVENT_CLOSE_FAILED:
				ZMQDIOLDBG_ << "ZMQ_EVENT_CLOSE_FAILED:"<<event.data.close_failed.addr;
				break;
			case ZMQ_EVENT_DISCONNECTED:
				ZMQDIOLDBG_ << "ZMQ_EVENT_DISCONNECTED:"<<event.data.disconnected.addr;
				break;
			default:
				break;
        }
    }
	ZMQDIOLAPP_ << "Leaving monitor loop";

    return NULL;
}

void ZMQDirectIOClient::freeObject(uint32_t hash, DirectIOClientConnection *connection) {
	releaseConnection(connection);
}

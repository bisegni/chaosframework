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

#include <boost/format.hpp>

#include <zmq.h>
#include <string.h>
#include <assert.h>     /* assert */

#define ZMQDIO_LOG_HEAD "["<<getName()<<"] - "

#define ZMQDIOLAPP_ LAPP_ << ZMQDIO_LOG_HEAD
#define ZMQDIOLDBG_ LDBG_ << ZMQDIO_LOG_HEAD << __FUNCTION__ << " - "
#define ZMQDIOLERR_ LERR_ << ZMQDIO_LOG_HEAD

using namespace chaos::common::direct_io::impl;

typedef boost::unique_lock<boost::shared_mutex>	ZMQDirectIOClientWriteLock;
typedef boost::shared_lock<boost::shared_mutex> ZMQDirectIOClientReadLock;

void ZMQDirectIOClientFree (void *data, void *hint) {
    free (data);
}

ZMQDirectIOClient::ZMQDirectIOClient(string alias):DirectIOClient(alias){
	priority_port = 0;
	service_port = 0;
	thread_run = false;
	zmq_context = NULL;
	socket_priority = NULL;
	socket_service = NULL;
};

ZMQDirectIOClient::~ZMQDirectIOClient(){
};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    ZMQDIOLAPP_ << "Allocating zmq context";
    std::string priority_identity = UUIDUtil::generateUUIDLite();
    std::string service_identity = UUIDUtil::generateUUIDLite();
	int err = 0;
	int output_buffer_dim = 1;
    int linger_period = 0;
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    
	ZMQDIOLAPP_ << "Inizilizing zmq implementation with zmq lib version = " << ZMQ_VERSION;
    ZMQDIOLAPP_ << "Set number of thread for the contex";
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    
    ZMQDIOLAPP_ << "Allocating priority socket";
    socket_priority = zmq_socket (zmq_context, ZMQ_DEALER);
    if(socket_priority == NULL) throw chaos::CException(1, "Error creating priority socket", __FUNCTION__);
    //set socket identity
    err = zmq_setsockopt (socket_priority, ZMQ_IDENTITY, priority_identity.c_str(), priority_identity.size());
	if(err) throw chaos::CException(err, "Error setting priority socket option", __FUNCTION__);
    
    err = zmq_setsockopt (socket_priority, ZMQ_LINGER, &linger_period, sizeof(int));
	if(err) throw chaos::CException(err, "Error setting linger on priority socket option", __FUNCTION__);
    //set output queue dime
	err = zmq_setsockopt (socket_priority, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
	if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on priority socket option", __FUNCTION__);
	
    ZMQDIOLAPP_ << "Allocating priority socket";
    socket_service = zmq_socket (zmq_context, ZMQ_DEALER);
    if(socket_service == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
    //set socket identity
    err = zmq_setsockopt(socket_service, ZMQ_IDENTITY, service_identity.c_str(), service_identity.size());
	if(err) throw chaos::CException(err, "Error setting service socket option", __FUNCTION__);
    
    err = zmq_setsockopt (socket_service, ZMQ_LINGER, &linger_period, sizeof(int));
	if(err) throw chaos::CException(err, "Error setting linger on service socket option", __FUNCTION__);
	//set output queue dime
	err = zmq_setsockopt (socket_service, ZMQ_SNDHWM, &output_buffer_dim, sizeof(int));
	if(err) throw chaos::CException(err, "Error setting ZMQ_SNDHWM on priority socket option", __FUNCTION__);

    ZMQDIOLAPP_ << "Allocating monitor socket";
    thread_run = true;
    socket_monitor = zmq_socket (zmq_context, ZMQ_PAIR);
    if(!socket_monitor) throw chaos::CException(err, "Error creating monitor socket", __FUNCTION__);
    
	err = zmq_socket_monitor(socket_priority, "inproc://monitor.req", ZMQ_EVENT_ALL);
    if(err) throw chaos::CException(err, "Error activating monitor on priority socket", __FUNCTION__);
    
    err = zmq_connect(socket_monitor, "inproc://monitor.req");
    if(err) throw chaos::CException(err, "Error connecting monitor socket", __FUNCTION__);

	monitor_thread.reset(new boost::thread(boost::bind(&ZMQDirectIOClient::monitorWorker, this)));
	
    ZMQDIOLAPP_ << "Initialized";
}

//! Deinit the implementation
void ZMQDirectIOClient::deinit() throw(chaos::CException) {
    int err = 0;
    ZMQDIOLAPP_ << "deinitialization";
    ZMQDIOLDBG_ << "Acquiring lock";
    ZMQDirectIOClientWriteLock lock(mutex_socket_manipolation);
    ZMQDIOLDBG_ << "Write lock acquired";

	ZMQDIOLAPP_ << "Close monitor socket";
	thread_run = false;
    err = zmq_close(socket_monitor);
    if(err) ZMQDIOLERR_ << "Error closing monitor socket";
    socket_monitor = NULL;
	
    ZMQDIOLAPP_ << "Close priority socket";
    err = zmq_close(socket_priority);
    if(err) ZMQDIOLERR_ << "Error closing priority socket";
    //zmq_close(socket_priority);
    socket_priority = NULL;
    
    ZMQDIOLAPP_ << "Close service socket";
    err = zmq_close(socket_service);
    if(err) ZMQDIOLERR_ << "Error closing service socket";

    //zmq_close(socket_service);
    socket_service = NULL;
	
    //destroy the zmq context
    ZMQDIOLAPP_ << "Destroyed zmq context";
    err = zmq_ctx_destroy(zmq_context);
    if(err) ZMQDIOLERR_ << "Error closing context";

    zmq_context = NULL;
    
    DirectIOClient::deinit();
    ZMQDIOLAPP_ << "Deinitialized";
}

//! check the connection with the endpoint for the two socket
void *ZMQDirectIOClient::monitorWorker() {
	zmq_event_t event;
    int err = 0;
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
    return NULL;
}

// send the data to the server layer on priority channel
int64_t ZMQDirectIOClient::sendPriorityData(DirectIODataPack *data_pack) {
    return writeToSocket(socket_priority, data_pack);
}

// send the data to the server layer on the service channel
int64_t ZMQDirectIOClient::sendServiceData(DirectIODataPack *data_pack) {
    return writeToSocket(socket_service, data_pack);
}

int64_t ZMQDirectIOClient::writeToSocket(void *socket, DirectIODataPack *data_pack) {
    assert(socket && data_pack);
	int err = 0;
	zmq_msg_t msg_header_data;
	zmq_msg_t msg_data;
	ZMQDirectIOClientReadLock lock(mutex_socket_manipolation);
	//send global header
	data_pack->header.dispatcher_header.raw_data = DIRECT_IO_SET_DISPATCHER_DATA(data_pack->header.dispatcher_header.raw_data);
	data_pack->header.channel_header_size = DIRECT_IO_SET_CHANNEL_HEADER_SIZE(data_pack->header.channel_header_size);
	data_pack->header.channel_data_size = DIRECT_IO_SET_CHANNEL_DATA_SIZE(data_pack->header.channel_data_size);
	
	//check what send
	switch(data_pack->header.dispatcher_header.fields.channel_part) {
		case DIRECT_IO_CHANNEL_PART_EMPTY:
			err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, ZMQ_DONTWAIT);
			break;
		case DIRECT_IO_CHANNEL_PART_HEADER_ONLY:
			err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, ZMQ_SNDMORE);
			if(err == -1) {
				return err;
			}
			err = zmq_msg_init_data (&msg_header_data, data_pack->channel_header_data, data_pack->header.channel_header_size, ZMQDirectIOClientFree, NULL);
			err = zmq_sendmsg(socket, &msg_header_data, ZMQ_DONTWAIT);
			err = zmq_msg_close(&msg_header_data);
			break;
		case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
			err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, ZMQ_SNDMORE);
			if(err == -1) {
				return err;
			}
			err = zmq_msg_init_data (&msg_data, data_pack->channel_data, data_pack->header.channel_data_size, ZMQDirectIOClientFree, NULL);
			err = zmq_sendmsg(socket, &msg_data, ZMQ_DONTWAIT);
			err = zmq_msg_close(&msg_data);
			break;
		case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
			err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, ZMQ_SNDMORE);
			if(err == -1) {
				return err;
			}
			err = zmq_msg_init_data (&msg_header_data, data_pack->channel_header_data, data_pack->header.channel_header_size, ZMQDirectIOClientFree, NULL);
			err = zmq_sendmsg(socket, &msg_header_data, ZMQ_SNDMORE);
			if(err == -1) {
				zmq_msg_close(&msg_header_data);
				return err;
			}
			err = zmq_msg_init_data (&msg_data, data_pack->channel_data, data_pack->header.channel_data_size, ZMQDirectIOClientFree, NULL);
			err = zmq_sendmsg(socket, &msg_data, 0);
			
			err = zmq_msg_close(&msg_header_data);
			err = zmq_msg_close(&msg_data);
			break;
	}
	delete (data_pack);
	//send data
    return err;

}

int ZMQDirectIOClient::addServer(std::string server_desc) {
	if(InizializableService::getServiceState() == ::chaos::utility::service_state_machine::InizializableServiceType::IS_DEINTIATED)
		return 0;
	ZMQDirectIOClientWriteLock lock(mutex_socket_manipolation);
	int err = 0;
	std::string priority_endpoint;
	std::string service_endpoint;
	std::string url;
	ServerFeeder::decoupleServerDescription(server_desc, priority_endpoint, service_endpoint);
	
	url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
	ZMQDIOLDBG_ << "connect to priority endpoint " << url;
	err = zmq_connect(socket_priority, url.c_str());
	if(err) {
		ZMQDIOLERR_ << "Error connecting priority socket to " << priority_endpoint;
		return err;
	}
	
	//add monitor on priority socket
	url = boost::str( boost::format("tcp://%1%") % service_endpoint);
	ZMQDIOLDBG_ << "connect to service endpoint " << url;
	err = zmq_connect(socket_service, url.c_str());
	if(err) {
		ZMQDIOLERR_ << "Error connecting service socket to " << service_endpoint;
		return err;
	}
	return 0;
}

int ZMQDirectIOClient::removeServer(std::string server_desc) {
	if(InizializableService::getServiceState() == ::chaos::utility::service_state_machine::InizializableServiceType::IS_DEINTIATED)
		return 0;
	ZMQDirectIOClientWriteLock lock(mutex_socket_manipolation);
	int err = 0;
	std::string priority_endpoint;
	std::string service_endpoint;
	std::string url;
	ServerFeeder::decoupleServerDescription(server_desc, priority_endpoint, service_endpoint);
	
	url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
	ZMQDIOLDBG_ << "connect to priority endpoint " << url;
	err = zmq_disconnect(socket_priority, url.c_str());
	if(err) {
		ZMQDIOLERR_ << "Error connecting priority socket to " << priority_endpoint;
		return err;
	}
	
	//add monitor on priority socket
	url = boost::str( boost::format("tcp://%1%") % service_endpoint);
	ZMQDIOLDBG_ << "connect to service endpoint " << url;
	err = zmq_disconnect(socket_service, url.c_str());
	if(err) {
		ZMQDIOLERR_ << "Error connecting service socket to " << service_endpoint;
		return err;
	}
	return 0;
}



/*
 *	ZMQDirectIOServer.cpp
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

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOServer.h>
#include <boost/format.hpp>

#include <zmq.h>

#define ZMQDIO_SRV_LOG_HEAD "["<<getName()<<"] - "

#define ZMQDIO_SRV_LAPP_ LAPP_ << ZMQDIO_SRV_LOG_HEAD
#define ZMQDIO_SRV_LDBG_ LDBG_ << ZMQDIO_SRV_LOG_HEAD
#define ZMQDIO_SRV_LERR_ LERR_ << ZMQDIO_SRV_LOG_HEAD


namespace chaos_data = chaos::common::data;

using namespace chaos::common::direct_io::impl;


ZMQDirectIOServer::ZMQDirectIOServer(std::string alias):DirectIOServer(alias) {
	zmq_context = NULL;
	priority_socket = NULL;
	service_socket = NULL;
	run_server = false;
};

ZMQDirectIOServer::~ZMQDirectIOServer(){
};

//! Initialize instance
void ZMQDirectIOServer::init(void *init_data) throw(chaos::CException) {
    DirectIOServer::init(init_data);
    chaos_data::CDataWrapper *init_cw = static_cast<chaos_data::CDataWrapper*>(init_data);
    if(!init_cw) throw chaos::CException(0, "No configration has been provided", __FUNCTION__);
	
	//get the port from configuration
	int32_t priority_port = init_cw->getInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_PRIORITY_PORT);
	if(priority_port <= 0) throw chaos::CException(0, "Bad priority port configured", __FUNCTION__);

	int32_t service_port = init_cw->getInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_SERVICE_PORT);
	if(service_port <= 0) throw chaos::CException(0, "Bad service port configured", __FUNCTION__);
	
	//create the endpoint strings
    priority_socket_bind_str = boost::str( boost::format("tcp://*:%1%") % priority_port);
    ZMQDIO_SRV_LAPP_ << "priority socket bind url: " << priority_socket_bind_str;
    
    service_socket_bind_str = boost::str( boost::format("tcp://*:%1%") % service_port);
    ZMQDIO_SRV_LAPP_ << "service socket bind url: " << service_socket_bind_str;
    
    //create the ZMQContext
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    
    //et the thread number
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    ZMQDIO_SRV_LAPP_ << "ZMQ context created";
}

//! Start the implementation
void ZMQDirectIOServer::start() throw(chaos::CException) {
    DirectIOServer::start();
    run_server = true;
    
    //queue thread
    ZMQDIO_SRV_LAPP_ << "Allocating and binding priority socket";
    priority_socket = zmq_socket (zmq_context, ZMQ_REP);
    int err = zmq_bind(priority_socket, priority_socket_bind_str.c_str());
    if(err) {
        std::string msg = boost::str( boost::format("Error binding priority socket to  %1% ") % priority_socket_bind_str);
        ZMQDIO_SRV_LAPP_ << msg;
        throw chaos::CException(-1, msg, __FUNCTION__);
    }
    
    ZMQDIO_SRV_LAPP_ << "Allocating and binding service socket";
    service_socket = zmq_socket (zmq_context, ZMQ_REP);
    err = zmq_bind(service_socket, service_socket_bind_str.c_str());
    if(err) {
        std::string msg = boost::str( boost::format("Error binding priority socket to  %1% ") % service_socket_bind_str);
        ZMQDIO_SRV_LAPP_ << msg;
        throw chaos::CException(-2, msg, __FUNCTION__);
    }
    
    ZMQDIO_SRV_LAPP_ << "Allocating threads for manage the requests";
    server_threads_group.add_thread(new thread(boost::bind(&ZMQDirectIOServer::worker, this, priority_socket, false)));
    server_threads_group.add_thread(new thread(boost::bind(&ZMQDirectIOServer::worker, this, service_socket, true)));
    ZMQDIO_SRV_LAPP_ << "Threads allocated and started";
}

//! Stop the implementation
void ZMQDirectIOServer::stop() throw(chaos::CException) {
    run_server = false;
    ZMQDIO_SRV_LAPP_ << "Stopping priority socket";
    if(priority_socket) zmq_close(priority_socket);
    
    ZMQDIO_SRV_LAPP_ << "Stopping service socket";
    if(service_socket) zmq_close(service_socket);
    
    //wiath all thread
     ZMQDIO_SRV_LAPP_ << "Join on all thread";
    server_threads_group.join_all();
    ZMQDIO_SRV_LAPP_ << "All thread stopped";
    DirectIOServer::stop();
}

//! Deinit the implementation
void ZMQDirectIOServer::deinit() throw(chaos::CException) {
    //serverThreadGroup.stopGroup(true);
    ZMQDIO_SRV_LAPP_ << "Deallocating zmq context";
    zmq_ctx_destroy (zmq_context);
    ZMQDIO_SRV_LAPP_ << "ZMQ Context deallocated";
    DirectIOServer::deinit();
}

void ZMQDirectIOServer::worker(void *socket, bool priority_service) {
    zmq_msg_t			request;
    int					err					= 0;
    DirectIODataPack	*data_pack			= NULL;
	void				*received_data		= NULL;
	uint32_t			received_data_size	= 0;
    //allcoate the delegate for this thread
    boost::function<void (DirectIODataPack*)> delegate = priority_service?
            boost::bind(&DirectIOHandler::serviceDataReceived, handler_impl, _1):
            boost::bind(&DirectIOHandler::priorityDataReceived, handler_impl, _1);
    while (run_server) {
        try {
			data_pack = new DirectIODataPack();
			//read header
            err = zmq_msg_init(&request);
            if(err == -1) {
                continue;
            }
            err = zmq_recvmsg(socket, &request, 0);
            if(err == -1 ||
				zmq_msg_size(&request) != DIRECT_IO_HEADER_SIZE) {
                zmq_msg_close(&request);
                continue;
            }
			received_data = zmq_msg_data(&request);
			received_data_size = (uint32_t)zmq_msg_size(&request);
            //data_pack->dio_pack_len = DIRECT_IO_GET_PACK_LEN(received_data);
			data_pack->header.dispatcher_raw_data = DIRECT_IO_GET_DISPATCHER_DATA(received_data);
			//close the request
            zmq_msg_close(&request);
			
			err = zmq_msg_init(&request);
            if(err == -1) {
                continue;
            }
            err = zmq_recvmsg(socket, &request, 0);
            if(err == -1 ||
			   zmq_msg_size(&request)==0) {
                zmq_msg_close(&request);
                continue;
            }
			//send data to first stage delegate
			data_pack->data = zmq_msg_data(&request);
			data_pack->data_size = (uint32_t)zmq_msg_size(&request);
            delegate(data_pack);
            //close the request
            zmq_msg_close(&request);

        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
    }
}

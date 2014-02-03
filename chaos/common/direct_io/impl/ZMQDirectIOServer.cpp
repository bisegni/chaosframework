//
//  DirectIOZMQServer.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOServer.h>

#include <boost/format.hpp>

#include <zmq.h>

#define ZMQDIO_SRV_LOG_HEAD "["<<getName()<<"] - "

#define ZMQDIO_SRV_LAPP_ LAPP_ << ZMQDIO_SRV_LOG_HEAD
#define ZMQDIO_SRV_LDBG_ LDBG_ << ZMQDIO_SRV_LOG_HEAD
#define ZMQDIO_SRV_LERR_ LERR_ << ZMQDIO_SRV_LOG_HEAD


namespace chaos_data = chaos::common::data;

using namespace chaos::common::direct_io::impl;


ZMQDirectIOServer::ZMQDirectIOServer(std::string alias):DirectIOServer(alias),run_server(false) {
};

ZMQDirectIOServer::~ZMQDirectIOServer(){
};

//! Initialize instance
void ZMQDirectIOServer::init(void *init_data) throw(chaos::CException) {
    DirectIOServer::init(init_data);
    chaos_data::CDataWrapper *init_cw = static_cast<chaos_data::CDataWrapper*>(init_data);
    if(!init_cw) throw chaos::CException(0, "No configration has been provided", __FUNCTION__);
	
	//get the port from configuration
	int32_t priority_port = init_cw->getInt32Value(DirectIOConfigurationKey::DIRECT_IO_PRIORITY_PORT);
	if(priority_port <= 0) throw chaos::CException(0, "Bad priority port configured", __FUNCTION__);

	int32_t service_port = init_cw->getInt32Value(DirectIOConfigurationKey::DIRECT_IO_SERVICE_PORT);
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
    int err = 0;
    zmq_msg_t request;
    
    //allcoate the delegate for this thread
    boost::function2<void, void*, uint32_t> delegate = priority_service?
            boost::bind(&DirectIOHandler::serviceDataReceived, handler_impl, _1, _2):
            boost::bind(&DirectIOHandler::priorityDataReceived, handler_impl, _1, _2);
    while (run_server) {
        try {

            err = zmq_msg_init(&request);
            if(err == -1) {
                continue;
            }
            err = zmq_recvmsg(socket, &request, 0);
            if(err == -1 || zmq_msg_size(&request)==0) {
                zmq_msg_close(&request);
                continue;
            }
            
            delegate(zmq_msg_data(&request), (uint32_t)zmq_msg_size(&request));
            
            zmq_msg_close(&request);
            //  Send reply back to client

            //err = zmq_msg_init_data(&response, (void*)result->getBufferPtr(), result->getBufferLen(), my_free, NULL);
            
            //err = zmq_sendmsg(receiver, &response, 0);
            //deallocate the data wrapper pack if it has been allocated
            //if(cdataWrapperPack) delete(cdataWrapperPack);
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
    }
}

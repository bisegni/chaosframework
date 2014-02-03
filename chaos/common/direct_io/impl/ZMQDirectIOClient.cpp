//
//  ZMQDirectIOClinet.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 31/01/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>

#include <boost/format.hpp>

#include <zmq.h>

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
	
	zmq_context = NULL;
	priority_socket = NULL;
	service_socket = NULL;
};

ZMQDirectIOClient::~ZMQDirectIOClient(){
};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    DirectIOClient::init(init_data);
    ZMQDIOLAPP_ << "Allocating zmq context";
    std::string priority_identity = UUIDUtil::generateUUIDLite();
    std::string service_identity = UUIDUtil::generateUUIDLite();
	
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    
    ZMQDIOLAPP_ << "Set number of thread for the contex";
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    
    ZMQDIOLAPP_ << "Allocating priority socket";
    priority_socket = zmq_socket (zmq_context, ZMQ_DEALER);
    if(priority_socket == NULL) throw chaos::CException(1, "Error creating priority", __FUNCTION__);
    //set socket identity
    zmq_setsockopt (priority_socket, ZMQ_IDENTITY, priority_identity.c_str(), priority_identity.size());
    
    ZMQDIOLAPP_ << "Allocating priority socket";
    service_socket = zmq_socket (zmq_context, ZMQ_DEALER);
    if(service_socket == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
    //set socket identity
    zmq_setsockopt (service_socket, ZMQ_IDENTITY, service_identity.c_str(), service_identity.size());
    
    ZMQDIOLAPP_ << "Initialized";
}

//! Start the implementation
void ZMQDirectIOClient::start() throw(chaos::CException) {
    DirectIOClient::start();
}

void ZMQDirectIOClient::switchMode(DirectIOConnectionSpreadType::DirectIOConnectionSpreadType direct_io_spread_mode) {
    int err = 0;
    std::string _priority_end_point;
	std::string _service_end_point;
    std::string priority_endpoint;
    std::string service_endpoint;
    //check if are already on the same spred type
    if(direct_io_spread_mode == current_spread_forwarding_type) return;
    current_spread_forwarding_type = direct_io_spread_mode;
    
    //lock for write
    ZMQDirectIOClientWriteLock lock(mutex_socket_manipolation);
    ZMQDIOLDBG_ << "Write lock acquired";
    switch (current_spread_forwarding_type) {
        case DirectIOConnectionSpreadType::DirectIOFailOver: {
            ZMQDIOLDBG_ << "Switch mod to DirectIOFailOver";
            std::vector< std::vector<std::string> > all_online_server;
            server_managment.getAllOnlineServer(all_online_server);
            for (std::vector< std::vector<std::string> >::iterator i = all_online_server.begin();
                 i != all_online_server.end();
                 i++) {
                priority_endpoint = boost::str( boost::format("tcp://%1%") % (*i)[0]);
                service_endpoint = boost::str( boost::format("tcp://%1%") % (*i)[1]);
                
                ZMQDIOLDBG_ << "connect to priority endpoint " << priority_endpoint;
                err = zmq_connect(priority_socket, priority_endpoint.c_str());
                if(err) {
                    ZMQDIOLERR_ << "Error connecting priority socket to " << priority_endpoint;
                }
                
                ZMQDIOLDBG_ << "connect to service endpoint " << service_endpoint;
                err = zmq_connect(service_socket, service_endpoint.c_str());
                if(err) {
                    ZMQDIOLERR_ << "Error connecting service socket to " << service_endpoint;
                }
            }
            break;
        }
            
        case DirectIOConnectionSpreadType::DirectIORoundRobin: {
            ZMQDIOLDBG_ << "Switch mod to DirectIORoundRobin";
            //try connecting to first server
            server_managment.getCurrentOnline(current_server_hash, _priority_end_point, _service_end_point);
            //connect the socket to server
            priority_endpoint = boost::str( boost::format("tcp://%1%") % _priority_end_point);
            service_endpoint = boost::str( boost::format("tcp://%1%") % _service_end_point);
            
            ZMQDIOLDBG_ << "connect to priority endpoint " << priority_endpoint;
            err = zmq_connect(priority_socket, priority_endpoint.c_str());
            if(err) {
                ZMQDIOLERR_ << "Error connecting priority socket to " << priority_endpoint;
            }
            
            ZMQDIOLDBG_ << "connect to service endpoint " << service_endpoint;
            err = zmq_connect(service_socket, service_endpoint.c_str());
            if(err) {
                ZMQDIOLERR_ << "Error connecting service socket to " << service_endpoint;
            }
            break;
        }
    }
    
}

//! Stop the implementation
void ZMQDirectIOClient::stop() throw(chaos::CException) {
    DirectIOClient::stop();
}

//! Deinit the implementation
void ZMQDirectIOClient::deinit() throw(chaos::CException) {
    ZMQDIOLAPP_ << "deinitialization";
    
    ZMQDirectIOClientWriteLock lock(mutex_socket_manipolation);
    ZMQDIOLDBG_ << "Write lock acquired";

    ZMQDIOLAPP_ << "Close priority socket";
    zmq_close(priority_socket);
    priority_socket = NULL;
    
    ZMQDIOLAPP_ << "Close service socket";
    zmq_close(service_socket);
    service_socket = NULL;
    
    //destroy the zmq context
    ZMQDIOLAPP_ << "Destroyed zmq context";
    zmq_ctx_destroy(zmq_context);
    zmq_context = NULL;
    
    DirectIOClient::deinit();
    ZMQDIOLAPP_ << "Deinitialized";
}

// send the data to the server layer on priority channel
uint32_t ZMQDirectIOClient::sendPriorityData(void *data_buffer, uint32_t data_size) {
    return writeToSocket(priority_socket, data_buffer, data_size);
}

// send the data to the server layer on the service channel
uint32_t ZMQDirectIOClient::sendServiceData(void *data_buffer, uint32_t data_size) {
    return writeToSocket(service_socket, data_buffer, data_size);
}

uint32_t ZMQDirectIOClient::receiveFromPriorityChannel(void **data_buffer, uint32_t *data_size) {
    return readFromSocket(priority_socket, data_buffer, data_size);
}

uint32_t ZMQDirectIOClient::receiveFromServiceChannel(void **data_buffer, uint32_t *data_size) {
    return readFromSocket(service_socket, data_buffer, data_size);
}

uint32_t ZMQDirectIOClient::writeToSocket(void *socket, void *data_buffer, uint32_t data_size) {
    assert(socket && data_buffer && data_size);
    
    ZMQDirectIOClientReadLock lock(mutex_socket_manipolation);
    ZMQDIOLDBG_ << "Read lock acquired";
    //send empty pack
    int err = zmq_send(priority_socket, "", 0, ZMQ_SNDMORE);
    if(err) {
        return err;
    }
    return zmq_send(priority_socket, data_buffer, data_size, 0);

}

uint32_t ZMQDirectIOClient::readFromSocket(void *socket, void **data_buffer, uint32_t *data_size) {
    assert(socket && data_buffer && data_size);
    
    ZMQDirectIOClientReadLock lock(mutex_socket_manipolation);
    zmq_msg_t message;
    zmq_msg_init (&message);
    int err = zmq_msg_recv (&message, socket, 0);
    if(err) {
        *data_buffer = zmq_msg_data(&message);
        *data_size = static_cast<uint32_t>(err);
    }
    zmq_msg_close(&message);
    return err;
}


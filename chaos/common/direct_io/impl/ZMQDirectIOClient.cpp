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
	socket_priority = NULL;
	socket_service = NULL;
};

ZMQDirectIOClient::~ZMQDirectIOClient(){
};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    DirectIOClient::init(init_data);
    ZMQDIOLAPP_ << "Allocating zmq context";
    std::string priority_identity = UUIDUtil::generateUUIDLite();
    std::string service_identity = UUIDUtil::generateUUIDLite();
	int err = 0;
	
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    
    ZMQDIOLAPP_ << "Set number of thread for the contex";
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    
    ZMQDIOLAPP_ << "Allocating priority socket";
    socket_priority = zmq_socket (zmq_context, ZMQ_DEALER);
    if(socket_priority == NULL) throw chaos::CException(1, "Error creating priority socket", __FUNCTION__);
    //set socket identity
    err = zmq_setsockopt (socket_priority, ZMQ_IDENTITY, priority_identity.c_str(), priority_identity.size());
	if(err) throw chaos::CException(err, "Error setting priority socket option", __FUNCTION__);
	
    ZMQDIOLAPP_ << "Allocating priority socket";
    socket_service = zmq_socket (zmq_context, ZMQ_DEALER);
    if(socket_service == NULL) throw chaos::CException(2, "Error creating service socket", __FUNCTION__);
    //set socket identity
    err = zmq_setsockopt(socket_service, ZMQ_IDENTITY, service_identity.c_str(), service_identity.size());
	if(err) throw chaos::CException(err, "Error setting service socket option", __FUNCTION__);
    ZMQDIOLAPP_ << "Initialized";
}

//! Start the implementation
void ZMQDirectIOClient::start() throw(chaos::CException) {
    DirectIOClient::start();
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
    ZMQBaseClass::closeSocketNoWhait(socket_priority);
    //zmq_close(socket_priority);
    socket_priority = NULL;
    
    ZMQDIOLAPP_ << "Close service socket";
    ZMQBaseClass::closeSocketNoWhait(socket_service);
    //zmq_close(socket_service);
    socket_service = NULL;
	
    //destroy the zmq context
    ZMQDIOLAPP_ << "Destroyed zmq context";
    zmq_ctx_destroy(zmq_context);
    zmq_context = NULL;
    
    DirectIOClient::deinit();
    ZMQDIOLAPP_ << "Deinitialized";
}


void ZMQDirectIOClient::switchMode(DirectIOConnectionSpreadType::DirectIOConnectionSpreadType direct_io_spread_mode) {
    int err = 0;
    std::string _priority_end_point;
	std::string _service_end_point;
    //check if are already on the same spred type
    if(direct_io_spread_mode == current_spread_forwarding_type) return;
    current_spread_forwarding_type = direct_io_spread_mode;
    
    //lock for write
    ZMQDirectIOClientWriteLock lock(mutex_socket_manipolation);
    ZMQDIOLDBG_ << "Write lock acquired";
    switch (current_spread_forwarding_type) {
        case DirectIOConnectionSpreadType::DirectIORoundRobin: {
            ZMQDIOLDBG_ << "Switch mod to DirectIORoundRobin";
            std::vector< std::vector<std::string> > all_online_server;
            server_managment.getAllOnlineServer(all_online_server);
            for (std::vector< std::vector<std::string> >::iterator i = all_online_server.begin();
                 i != all_online_server.end();
                 i++) {
                current_priority_endpoint = boost::str( boost::format("tcp://%1%") % (*i)[0]);
                current_service_endpoint = boost::str( boost::format("tcp://%1%") % (*i)[1]);
                
                ZMQDIOLDBG_ << "connect to priority endpoint " << current_priority_endpoint;
                err = zmq_connect(socket_priority, current_priority_endpoint.c_str());
                if(err) {
                    ZMQDIOLERR_ << "Error connecting priority socket to " << current_priority_endpoint;
                }
                
                ZMQDIOLDBG_ << "connect to service endpoint " << current_service_endpoint;
                err = zmq_connect(socket_service, current_service_endpoint.c_str());
                if(err) {
                    ZMQDIOLERR_ << "Error connecting service socket to " << current_service_endpoint;
                }
            }
            break;
        }
            
        case DirectIOConnectionSpreadType::DirectIOFailOver: {
            ZMQDIOLDBG_ << "Switch mod to DirectIOFailOver";
            //try connecting to first server
            server_managment.getCurrentOnline(current_server_hash, _priority_end_point, _service_end_point);
            //connect the socket to server
            current_priority_endpoint = boost::str( boost::format("tcp://%1%") % _priority_end_point);
            current_service_endpoint = boost::str( boost::format("tcp://%1%") % _service_end_point);
            
            ZMQDIOLDBG_ << "connect to priority endpoint " << current_priority_endpoint;
            err = zmq_connect(socket_priority, current_priority_endpoint.c_str());
            if(err) {
                ZMQDIOLERR_ << "Error connecting priority socket to " << current_priority_endpoint;
            }
            
            ZMQDIOLDBG_ << "connect to service endpoint " << current_service_endpoint;
            err = zmq_connect(socket_service, current_service_endpoint.c_str());
            if(err) {
                ZMQDIOLERR_ << "Error connecting service socket to " << current_service_endpoint;
            }
            break;
        }
    }
    
}

// send the data to the server layer on priority channel
uint32_t ZMQDirectIOClient::sendPriorityData(DirectIODataPack *data_pack) {
    return writeToSocket(socket_priority, data_pack);
}

// send the data to the server layer on the service channel
uint32_t ZMQDirectIOClient::sendServiceData(DirectIODataPack *data_pack) {
    return writeToSocket(socket_service, data_pack);
}

uint32_t ZMQDirectIOClient::writeToSocket(void *socket, DirectIODataPack *data_pack) {
    assert(socket && data_pack);
    ZMQDirectIOClientReadLock lock(mutex_socket_manipolation);
	//send header
	int err = zmq_send(socket, &data_pack->header.dispatcher_raw_data, 4, ZMQ_SNDMORE);
	if(err) {
		return err;
	}
	//send data
    return zmq_send(socket, data_pack->data, data_pack->data_size, 0);

}



//
//  ZMQDirectIOClientConnection.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 10/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>


#include <boost/format.hpp>

#include <string.h>


typedef boost::unique_lock<boost::shared_mutex>	ZMQDirectIOClientConnectionWriteLock;
typedef boost::shared_lock<boost::shared_mutex> ZMQDirectIOClientConnectionReadLock;

using namespace chaos::common::direct_io::impl;

#define ZMQDIO_CONNECTION_LOG_HEAD "[ZMQDirectIOClientConnection] - "

#define ZMQDIO_CONNECTION_LAPP_ LAPP_ << ZMQDIO_CONNECTION_LOG_HEAD
#define ZMQDIO_CONNECTION_LDBG_ LDBG_ << ZMQDIO_CONNECTION_LOG_HEAD << __FUNCTION__ << " - "
#define ZMQDIO_CONNECTION_LERR_ LERR_ << ZMQDIO_CONNECTION_LOG_HEAD << __FUNCTION__ << "(" << __LINE__ << ") - "

ZMQDirectIOClientConnection::ZMQDirectIOClientConnection(std::string server_description, void *_socket_priority, void *_socket_service, uint16_t endpoint):
DirectIOClientConnection(server_description, endpoint),
socket_priority(_socket_priority),
socket_service(_socket_service) {}


ZMQDirectIOClientConnection::~ZMQDirectIOClientConnection() {}

// send the data to the server layer on priority channel
int64_t ZMQDirectIOClientConnection::sendPriorityData(DirectIODataPack *data_pack,
                                                      DirectIODeallocationHandler *header_deallocation_handler,
                                                      DirectIODeallocationHandler *data_deallocation_handler,
                                                      DirectIODataPack **synchronous_answer) {
    return writeToSocket(socket_priority,
                         priority_identity,
                         completeDataPack(data_pack),
                         header_deallocation_handler,
                         data_deallocation_handler,
                         synchronous_answer);
}

// send the data to the server layer on the service channel
int64_t ZMQDirectIOClientConnection::sendServiceData(DirectIODataPack *data_pack,
                                                     DirectIODeallocationHandler *header_deallocation_handler,
                                                     DirectIODeallocationHandler *data_deallocation_handler,
                                                     DirectIODataPack **synchronous_answer) {
    return writeToSocket(socket_service,
                         service_identity,
                         completeDataPack(data_pack),
                         header_deallocation_handler,
                         data_deallocation_handler,
                         synchronous_answer);
}

//send data with zmq tech
int64_t ZMQDirectIOClientConnection::writeToSocket(void *socket,
                                                   std::string& identity,
                                                   DirectIODataPack *data_pack,
                                                   DirectIODeallocationHandler *header_deallocation_handler,
                                                   DirectIODeallocationHandler *data_deallocation_handler,
                                                   DirectIODataPack **synchronous_answer) {
    assert(socket && data_pack);
    int err = 0;
    
    if((err = sendDatapack(socket,
                           data_pack,
                           header_deallocation_handler,
                           data_deallocation_handler))) {
        ZMQDIO_CONNECTION_LERR_ << "Error sending datapack with code:" << err;
    } else if(data_pack->header.dispatcher_header.fields.synchronous_answer) {
        //we need an aswer
        if((err = reveiceDatapack(socket,
                                 synchronous_answer))) {
            ZMQDIO_CONNECTION_LERR_ << "Error receiving answer datapack with code:" << err;
        }
    }
    //remove datapack;
    free(data_pack);
    return err;
}
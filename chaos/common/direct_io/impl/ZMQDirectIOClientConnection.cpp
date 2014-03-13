//
//  ZMQDirectIOClientConnection.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 10/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/impl/ZMQDirectIOClientConnection.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>

#include <boost/format.hpp>

#include <zmq.h>
#include <string.h>

typedef boost::unique_lock<boost::shared_mutex>	ZMQDirectIOClientConnectionWriteLock;
typedef boost::shared_lock<boost::shared_mutex> ZMQDirectIOClientConnectionReadLock;

using namespace chaos::common::direct_io::impl;

#define ZMQDIO_CONNECTION_LOG_HEAD "[ZMQDirectIOClientConnection] - "

#define ZMQDIO_CONNECTION_LAPP_ LAPP_ << ZMQDIO_CONNECTION_LOG_HEAD
#define ZMQDIO_CONNECTION_LDBG_ LDBG_ << ZMQDIO_CONNECTION_LOG_HEAD << __FUNCTION__ << " - "
#define ZMQDIO_CONNECTION_LERR_ LERR_ << ZMQDIO_CONNECTION_LOG_HEAD

void ZMQDirectIOClientConnectionFree (void *data, void *hint) {
    free(data);
}

ZMQDirectIOClientConnection::ZMQDirectIOClientConnection(std::string server_description, void *_socket_priority, void *_socket_service):
DirectIOClientConnection(server_description), socket_priority(_socket_priority), socket_service(_socket_service), monitor_info(NULL) {
}


ZMQDirectIOClientConnection::~ZMQDirectIOClientConnection() {
	
}

// send the data to the server layer on priority channel
int64_t ZMQDirectIOClientConnection::sendPriorityData(DirectIODataPack *data_pack) {
    return writeToSocket(socket_priority, data_pack);
}

// send the data to the server layer on the service channel
int64_t ZMQDirectIOClientConnection::sendServiceData(DirectIODataPack *data_pack) {
    return writeToSocket(socket_service, data_pack);
}

int64_t ZMQDirectIOClientConnection::writeToSocket(void *socket, DirectIODataPack *data_pack) {
    assert(socket && data_pack);
	int err = 0;
	zmq_msg_t msg_header_data;
	zmq_msg_t msg_data;
	ZMQDirectIOClientConnectionReadLock lock(mutex_socket_manipolation);
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
			err = zmq_msg_init_data (&msg_header_data, data_pack->channel_header_data, data_pack->header.channel_header_size, ZMQDirectIOClientConnectionFree, NULL);
			err = zmq_sendmsg(socket, &msg_header_data, ZMQ_DONTWAIT);
			err = zmq_msg_close(&msg_header_data);
			break;
		case DIRECT_IO_CHANNEL_PART_DATA_ONLY:
			err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, ZMQ_SNDMORE);
			if(err == -1) {
				return err;
			}
			err = zmq_msg_init_data (&msg_data, data_pack->channel_data, data_pack->header.channel_data_size, ZMQDirectIOClientConnectionFree, NULL);
			err = zmq_sendmsg(socket, &msg_data, ZMQ_DONTWAIT);
			err = zmq_msg_close(&msg_data);
			break;
		case DIRECT_IO_CHANNEL_PART_HEADER_DATA:
			err = zmq_send(socket, &data_pack->header, DIRECT_IO_HEADER_SIZE, ZMQ_SNDMORE);
			if(err == -1) {
				return err;
			}
			err = zmq_msg_init_data (&msg_header_data, data_pack->channel_header_data, data_pack->header.channel_header_size, ZMQDirectIOClientConnectionFree, NULL);
			err = zmq_sendmsg(socket, &msg_header_data, ZMQ_SNDMORE);
			if(err == -1) {
				zmq_msg_close(&msg_header_data);
				return err;
			}
			err = zmq_msg_init_data (&msg_data, data_pack->channel_data, data_pack->header.channel_data_size, ZMQDirectIOClientConnectionFree, NULL);
			err = zmq_sendmsg(socket, &msg_data, 0);
			
			err = zmq_msg_close(&msg_header_data);
			err = zmq_msg_close(&msg_data);
			break;
	}
	delete (data_pack);
	//send data
    return err;
}

int ZMQDirectIOClientConnection::addServer(std::string server_desc) {
	ZMQDirectIOClientConnectionWriteLock lock(mutex_socket_manipolation);
	int err = 0;
	std::string priority_endpoint;
	std::string service_endpoint;
	std::string url;
	ServerFeeder::decoupleServerDescription(server_desc, priority_endpoint, service_endpoint);
	
	url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
	ZMQDIO_CONNECTION_LAPP_ << "connect to priority endpoint " << url;
	err = zmq_connect(socket_priority, url.c_str());
	if(err) {
		ZMQDIO_CONNECTION_LERR_ << "Error connecting priority socket to " << priority_endpoint;
		return err;
	}
	
	//add monitor on priority socket
	url = boost::str( boost::format("tcp://%1%") % service_endpoint);
	ZMQDIO_CONNECTION_LAPP_ << "connect to service endpoint " << url;
	err = zmq_connect(socket_service, url.c_str());
	if(err) {
		ZMQDIO_CONNECTION_LERR_ << "Error connecting service socket to " << service_endpoint;
		return err;
	}
	return 0;
}

int ZMQDirectIOClientConnection::removeServer(std::string server_desc) {
	ZMQDirectIOClientConnectionWriteLock lock(mutex_socket_manipolation);
	int err = 0;
	std::string priority_endpoint;
	std::string service_endpoint;
	std::string url;
	ServerFeeder::decoupleServerDescription(server_desc, priority_endpoint, service_endpoint);
	
	url = boost::str( boost::format("tcp://%1%") % priority_endpoint);
	ZMQDIO_CONNECTION_LAPP_ << "connect to priority endpoint " << url;
	err = zmq_disconnect(socket_priority, url.c_str());
	if(err) {
		ZMQDIO_CONNECTION_LERR_ << "Error connecting priority socket to " << priority_endpoint;
		return err;
	}
	
	//add monitor on priority socket
	url = boost::str( boost::format("tcp://%1%") % service_endpoint);
	ZMQDIO_CONNECTION_LAPP_ << "connect to service endpoint " << url;
	err = zmq_disconnect(socket_service, url.c_str());
	if(err) {
		ZMQDIO_CONNECTION_LERR_ << "Error connecting service socket to " << service_endpoint;
		return err;
	}
	return 0;
}
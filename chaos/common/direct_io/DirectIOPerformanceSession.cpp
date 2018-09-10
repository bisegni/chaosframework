//
//  DirectIOPerformanceSession.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <chaos/common/direct_io/DirectIOPerformanceSession.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/DirectIODispatcher.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
using namespace chaos::common::direct_io;

#define DIPL_LOG_HEAD "[DirectIOPerformanceSession] - "

#define DIPLLAPP_ LAPP_ << DIPL_LOG_HEAD
#define DIPLLDBG_ LDBG_ << DIPL_LOG_HEAD << __FUNCTION__
#define DIPLLERR_ LERR_ << DIPL_LOG_HEAD

DirectIOPerformanceSession::DirectIOPerformanceSession(DirectIOClientConnection *_client_connection,  DirectIOServerEndpoint *_server_endpoint):
 client_connection(_client_connection), server_endpoint(_server_endpoint), rtt_queue(1){
}

DirectIOPerformanceSession::~DirectIOPerformanceSession() {
}

void DirectIOPerformanceSession::setConnectionHandler(DirectIOClientConnectionEventHandler *event_handler) {
	if(!client_connection) return;
	client_connection->setEventHandler(event_handler);
}

// Initialize instance
void DirectIOPerformanceSession::init(void *init_data)  {
	//check the client and server
	if(!client_connection) throw CException(-1 ,"The client instance as not been set", __PRETTY_FUNCTION__);
	if(!server_endpoint) throw CException(-2 ,"Error initializing the endpoint", __PRETTY_FUNCTION__);
		
	//get the connection to the client
	DIPLLAPP_ << "allocating the client channel";
	//get the channels
	client_channel = dynamic_cast<channel::DirectIOPerformanceClientChannel*>(client_connection->getNewChannelInstance(std::string("DirectIOPerformanceClientChannel")));
	if(!client_channel) throw CException(-3 ,"Error creating the client channel", __PRETTY_FUNCTION__);
	
	DIPLLAPP_ << "allocating the server channel";
	server_channel = dynamic_cast<channel::DirectIOPerformanceServerChannel*>(server_endpoint->getNewChannelInstance(std::string("DirectIOPerformanceServerChannel")));
	if(!server_channel) throw CException(-3 ,"Error creating the server channel", __PRETTY_FUNCTION__);
	server_channel->setHandler(this);
}

// Deinit the implementation
void DirectIOPerformanceSession::deinit()  {
	//release the channel
	
	if(server_endpoint && server_channel) {
		DIPLLAPP_ << "deallocating the server channel";
		server_endpoint->releaseChannelInstance(server_channel);
	} else {
		DIPLLAPP_ << "no server channel allocated";
	}
	
	if(client_connection && client_channel) {
		DIPLLAPP_ << "deallocating the client channel";
		client_connection->releaseChannelInstance(client_channel);
	} else {
		DIPLLAPP_ << "no client channel allocated";
	}
}

int64_t DirectIOPerformanceSession::sendRttTest(uint32_t ms_timeout) {
	int64_t result =0;
	result = client_channel->sendRoundTripMessage();
	return result;
}

RttResultFetcher *DirectIOPerformanceSession::getRttResultQueue() {
	ResultFetcher<channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr> *result = new ResultFetcher<channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr>(&rtt_queue);
	return result;
}

void DirectIOPerformanceSession::handleReqRoundTripRequest(channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request) {
	//send the answer
	DIPLLAPP_ << "pre req";
	if(client_channel->answerRoundTripMessage(rtt_request) == -1) {
		DIPLLERR_ << "Error sending rtt answer";
	}
	DIPLLAPP_ << "post req";
}

void DirectIOPerformanceSession::handleRespRoundTripRequest(channel::opcode_headers::DirectIOPerformanceChannelHeaderOpcodeRoundTripPtr rtt_request) {
	//push result on queue
	rtt_queue.push(rtt_request);
}

/*
 *	DirectIOClientConnection.h
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

#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/utility/UUIDUtil.h>

#include <boost/format.hpp>
using namespace chaos::common::direct_io;

#define DIOVCC_LOG_HEAD "[DirectIOVirtualClientChannel: "<< server_description <<"] - "

#define DIOVCCLAPP_ LAPP_ << DIOVCC_LOG_HEAD
#define DIOVCCLDBG_ LDBG_ << DIOVCC_LOG_HEAD
#define DIOVCCLERR_ LERR_ << DIOVCC_LOG_HEAD

// current client ip in string form
std::string DirectIOClientConnection::my_str_ip;

// current client ip in 64 bit form
uint64_t DirectIOClientConnection::my_i64_ip = 0;

void DirectIOClientConnection::freeSentData(void *data, void *hint) {
	channel::DisposeSentMemoryInfo *free_info = static_cast<channel::DisposeSentMemoryInfo*>(hint);
	free_info->channel->freeSentData(data, *free_info);
	free(hint);
}

DirectIOClientConnection::DirectIOClientConnection(std::string _server_description, uint16_t _endpoint):server_description(_server_description), endpoint(_endpoint), event_handler(NULL) {
	//set the default connection hash
    //generate random hash from uuid lite
    std::string unique_uuid = UUIDUtil::generateUUIDLite();
    
	url = boost::str( boost::format("%1%|%2%") % server_description % endpoint);
    unique_hash = chaos::common::data::cache::FastHash::hash(unique_uuid.c_str(), unique_uuid.size(), 0);
}

DirectIOClientConnection::~DirectIOClientConnection() {
	
}

const char * DirectIOClientConnection::getServerDescription() {
	return server_description.c_str();
}

std::string DirectIOClientConnection::getURL() {
	return url;
}

std::string DirectIOClientConnection::getStrIp() {
    return my_str_ip;
}
uint64_t DirectIOClientConnection::getI64Ip() {
    return my_i64_ip;
}

uint32_t DirectIOClientConnection::getUniqueHash() {
    return unique_hash;
}

std::string	DirectIOClientConnection::getCustomStringIdentification() {
	return custom_string_identification;
}

void DirectIOClientConnection::setCustomStringIdentification(std::string _custom_string_identificaiton) {
	custom_string_identification = _custom_string_identificaiton;
}

void DirectIOClientConnection::setEventHandler(DirectIOClientConnectionEventHandler *_event_handler) {
	event_handler = _event_handler;
}

DirectIOClientConnectionStateType::DirectIOClientConnectionStateType DirectIOClientConnection::getState() {
	return current_state;
}

void DirectIOClientConnection::lowLevelManageEvent(DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event_state) {
	current_state = event_state;
	DEBUG_CODE( switch(current_state)  {
		case DirectIOClientConnectionStateType::DirectIOClientConnectionEventConnected:
			DIOVCCLDBG_ << "Set to state ->  DirectIOClientConnectionEventConnected";
			break;
		case DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected:
			DIOVCCLDBG_ << "Set to state ->  DirectIOClientConnectionEventDisconnected";
			break;
	});
	//forward event to the handler
	if(event_handler) event_handler->handleEvent(this, current_state);
}

// New channel allocation by name
channel::DirectIOVirtualClientChannel *DirectIOClientConnection::getNewChannelInstance(std::string channel_name) {
	channel::DirectIOVirtualClientChannel *channel = chaos::ObjectFactoryRegister<channel::DirectIOVirtualClientChannel>::getInstance()->getNewInstanceByName(channel_name.c_str());
    //sub class method for register the instance
    if(channel) {
		channel->client_instance = this;
		DICKeyObjectContainer::registerElement(channel->channel_route_index, channel);
    }
	return channel;
}

// New channel allocation by name
void DirectIOClientConnection::releaseChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance) {
	if(channel_instance) {
        DICKeyObjectContainer::deregisterElementKey(channel_instance->channel_route_index);
		channel_instance->client_instance = NULL;
        delete(channel_instance);
    }
}

void DirectIOClientConnection::freeObject(unsigned int hash, DirectIOClientConnection *connection) {
	//releaseConnection(connection);
}
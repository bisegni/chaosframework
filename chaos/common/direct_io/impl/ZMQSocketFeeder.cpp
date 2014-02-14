/*
 *	ZMQSocketFeeder.cpp
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

#include <algorithm>

#include <vector>

#include <chaos/common/data/cache/FastHash.h>
#include <chaos/common/direct_io/impl/ZMQSocketFeeder.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>

#include <zmq.h>

#define ZMQ_MAX_SOCKET 100

using namespace chaos::common::direct_io::impl;

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex DirectIOHostNameRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}:[0-9]{4,5}");
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const boost::regex DirectIOIPAndPortRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}:[0-9]{4,5}");

ZMQSocketFeeder::ZMQSocketFeeder():socket_slot(NULL), socket_allocated(0) {
		//allocate array space
	size_t size_to_allocate = sizeof(socket_info*)*ZMQ_MAX_SOCKET;
	socket_slot = (socket_info**)malloc(size_to_allocate);
	std::memset(socket_slot, 0, size_to_allocate);
}

ZMQSocketFeeder::~ZMQSocketFeeder() {
	if(socket_slot) {
		for(int idx = 0; idx < ZMQ_MAX_SOCKET; idx++) {
			if(socket_slot[idx]) {
				if(socket_slot[idx]->socket) zmq_close(socket_slot[idx]->socket);
				delete socket_slot[idx];
			}
		}
		free(socket_slot);
	}
}

void ZMQSocketFeeder::decoupleServerDescription(std::string server_desc, socket_info *socket_info) {
	std::vector<std::string> server_desc_tokens;
	boost::algorithm::split(server_desc_tokens, server_desc, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	
	//create the two endpoint description
	std::strncpy(socket_info->endpoints.priority, boost::str( boost::format("%1%:%2%") % server_desc_tokens[0] % server_desc_tokens[1]).c_str(), 255);
	std::strncpy(socket_info->endpoints.service, boost::str( boost::format("%1%:%2%") % server_desc_tokens[0] % server_desc_tokens[2]).c_str(), 255);
	socket_slot[socket_allocated]->hash_host_ip = data::cache::FastHash::hash(server_desc_tokens[0].c_str(), server_desc_tokens[0].size(), 0);
}

bool ZMQSocketFeeder::addServer(std::string server_desc) {
	if(socket_allocated+1 == ZMQ_MAX_SOCKET) return false;
	
	//create socket description
	socket_slot[socket_allocated] = new socket_info();
	
	//clear description
	std::memset(socket_slot[socket_allocated], 0, sizeof(socket_info));
	
	socket_slot[socket_allocated]->index = socket_allocated;
	
	boost::algorithm::trim(server_desc);
	std::string normalized_server_desc = boost::algorithm::to_lower_copy(server_desc);
	
	//check if the description is well formed
	if(!regex_match(server_desc, DirectIOHostNameRegExp) && !regex_match(server_desc, DirectIOIPAndPortRegExp)) return false;
	
	//decoplue endpoints
	decoupleServerDescription(server_desc, socket_slot[socket_allocated]);
	
	//put this server on the online queue
	online_socket.push(socket_allocated);
	return true;
}

void *ZMQSocketFeeder::getNextSocket() {
	return NULL;
}

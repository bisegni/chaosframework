/*
 *	DirectIOClient.cpp
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
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::direct_io;
namespace chaos_data = chaos::common::data;

namespace b_algo = boost::algorithm;

#define DIO_LOG_HEAD "["<<getName()<<"] - "

#define DIOLAPP_ LAPP_ << DIO_LOG_HEAD
#define DIOLDBG_ LDBG_ << DIO_LOG_HEAD << __FUNCTION__
#define DIOLERR_ LERR_ << DIO_LOG_HEAD

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex DataProxyServerDescriptionIPRegExp("[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}:[0-9]{4,5}\\|[0-9]{1,3}");
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const boost::regex DataProxyServerDescriptionHostRegExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}:[0-9]{4,5}\\|[0-9]{1,3}");


DirectIOClient::DirectIOClient(string alias):NamedService(alias) {
	
}

DirectIOClient::~DirectIOClient() {
}

void DirectIOClient::forwardEventToClientConnection(DirectIOClientConnection *client, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event_type) {
	client->lowLevelManageEvent(event_type);
}

bool DirectIOClient::decodeServerDescirptionWithEndpoint(std::string server_description_endpoint, std::string& server_description, uint16_t& endpoint) {
    std::vector<string> tokens;
    
    if(!boost::regex_match(server_description_endpoint, DataProxyServerDescriptionIPRegExp) &&
       !boost::regex_match(server_description_endpoint, DataProxyServerDescriptionHostRegExp)) {
        DIOLERR_ << "server description " << server_description_endpoint << " non well formed";
        return false;
    }
    
    boost::algorithm::split(tokens, server_description_endpoint, boost::algorithm::is_any_of("|"), boost::algorithm::token_compress_on);

    server_description = tokens[0];
    endpoint = boost::lexical_cast<uint16_t>(tokens[1]);
    return true;
}

DirectIOClientConnection *DirectIOClient::getNewConnection(std::string server_description_with_endpoint) {
    uint16_t endpoint;
    std::string server_description;
    DEBUG_CODE(DIOLDBG_ << "Requested a new connection for a server description " << server_description_with_endpoint;)
    if(decodeServerDescirptionWithEndpoint(server_description_with_endpoint, server_description, endpoint)) {
        DEBUG_CODE(DIOLDBG_ << "scomposed into server description " << server_description << " and endpoint " << endpoint;)
        return getNewConnection(server_description, endpoint);
    } else return NULL;
}
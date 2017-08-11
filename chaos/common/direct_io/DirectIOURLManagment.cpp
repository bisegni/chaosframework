/*
 *	DirectIOURLManagment.cpp
 *	!CHAOS
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

#include <chaos/common/direct_io/DirectIOURLManagment.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::direct_io;


//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const char * const DirectIOHostIPAndEndpoint = "[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}:[0-9]{4,5}\\|[0-9]{1,3}";
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const char * const DirectIOServerDescriptionHostAndEndpoint = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}:[0-9]{4,5}\\|[0-9]{1,3}";
//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const char * const DirectIOHostName = "[a-zA-Z0-9]+(.[a-zA-Z0-9]+)+:[0-9]{4,5}:[0-9]{4,5}";
//! Regular expression for check server endpoint with the sintax ip:[priority_port:service_port]
static const char * const DirectIOIPAndPort = "\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b:[0-9]{4,5}:[0-9]{4,5}";

bool DirectIOURLManagment::checkURL(const std::string& url) {
    boost::regex DirectIOHostIPAndEndpointRegExp(DirectIOHostIPAndEndpoint);
    boost::regex DirectIOServerDescriptionHostAndEndpointRegExp(DirectIOServerDescriptionHostAndEndpoint);
	return	boost::regex_match(url, DirectIOHostIPAndEndpointRegExp) ||
	boost::regex_match(url, DirectIOServerDescriptionHostAndEndpointRegExp);
}


bool DirectIOURLManagment::decoupleServerDescription(const std::string& server_desc,
													 std::string& priority_desc,
													 std::string& service_desc) {
	std::vector<std::string> server_desc_tokens;
    boost::regex DirectIOHostNameRegExp(DirectIOHostName);
    boost::regex DirectIOIPAndPortRegExp(DirectIOIPAndPort);
	if(!boost::regex_match(server_desc, DirectIOHostNameRegExp) &&
       !boost::regex_match(server_desc, DirectIOIPAndPortRegExp)) {
        return false;
    }
	boost::algorithm::split(server_desc_tokens, server_desc, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	
	//create the two servers description
	priority_desc = boost::str( boost::format("%1%:%2%") % server_desc_tokens[0] % server_desc_tokens[1]);
	service_desc = boost::str( boost::format("%1%:%2%") % server_desc_tokens[0] % server_desc_tokens[2]);
	return true;
}

bool DirectIOURLManagment::decoupleServerDescription(const std::string& server_desc,
													 std::vector<std::string>& servers_desc) {
	std::string p_desc;
	std::string s_desc;
	if(! decoupleServerDescription(server_desc, p_desc, s_desc)) {
		return false;
	}
	servers_desc.push_back(p_desc);
	servers_desc.push_back(s_desc);
	return true;
}


bool DirectIOURLManagment::decodeServerDescriptionWithEndpoint(const std::string& server_description_endpoint,
															   std::string& server_description,
															   uint16_t& endpoint) {
    std::vector<std::string> tokens;
    
    if(!checkURL(server_description_endpoint)) {
        return false;
    }
    
    boost::algorithm::split(tokens, server_description_endpoint, boost::algorithm::is_any_of("|"), boost::algorithm::token_compress_on);
	
    server_description = tokens[0];
    endpoint = boost::lexical_cast<uint16_t>(tokens[1]);
    return true;
}

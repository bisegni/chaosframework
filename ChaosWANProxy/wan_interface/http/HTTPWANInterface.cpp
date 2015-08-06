/*
 *	HTTPWANInterface.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#include "HTTPWANInterface.h"
#include "HTTPWANInterfaceStringResponse.h"

#include <chaos/common/utility/TimingUtil.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <json/json.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::wan_proxy::wan_interface;
using namespace chaos::wan_proxy::wan_interface::http;

#define API_PREFIX_V1 "/api/v1"
#define API_PATH_REGEX_V1(p) API_PREFIX_V1 p

#define HTTWANINTERFACE_LOG_HEAD "["<<getName()<<"] - "

#define HTTWAN_INTERFACE_APP_ INFO_LOG(HTTPWANInterface)
#define HTTWAN_INTERFACE_DBG_ DBG_LOG(HTTPWANInterface)
#define HTTWAN_INTERFACE_ERR_ ERR_LOG(HTTPWANInterface)

static const boost::regex REG_API_URL_FORMAT(API_PATH_REGEX_V1("((/[a-zA-Z0-9_]+))*")); //"/api/v1((/[a-zA-Z0-9_]+))*"


static int do_i_handle(struct mg_connection *connection) {
	if (connection->server_param != NULL) {
		return ((HTTPWANInterface *)connection->server_param)->handle(connection);
	}
	return 0;
}

/**
 * The handlers below are written in C to do the binding of the C mongoose with
 * the C++ API
 */
static int event_handler(struct mg_connection *connection) {
	if (connection->server_param != NULL) {
		((HTTPWANInterface *)connection->server_param)->process(connection);
	}
	return 1;
}

static void flush_response(struct mg_connection *connection,
						   AbstractWANInterfaceResponse *response) {
	CHAOS_ASSERT(connection && response)
	mg_send_status(connection, response->getCode());
	
	for(std::map<std::string, std::string>::const_iterator it = response->getHeader().begin();
		it != response->getHeader().end();
		it++){
		mg_send_header(connection, it->first.c_str(), it->second.c_str());
	}
	
	uint32_t body_len = 0;
	const char * body = response->getBody(body_len);
	mg_send_data(connection, body, body_len);
}

DEFINE_CLASS_FACTORY(HTTPWANInterface, AbstractWANInterface);
HTTPWANInterface::HTTPWANInterface(const string& alias):
AbstractWANInterface(alias),
thread_number(0) {}

HTTPWANInterface::~HTTPWANInterface() {}

//inherited method
void HTTPWANInterface::init(void *init_data) throw(CException) {
	//! forward message to superclass
	AbstractWANInterface::init(init_data);
	
	//clear in case last deinit fails
	http_server_list.clear();
	
	//check for parameter
	if(getParameter()[OPT_HTTP_PORT].isNull() ||
       !getParameter()[OPT_HTTP_PORT].isInt()) {
		std::string err = "Port for http interface as not be set!";
		HTTWAN_INTERFACE_ERR_ << err;
		throw chaos::CException(-1, err, __PRETTY_FUNCTION__);
	} else {
		service_port = getParameter()[OPT_HTTP_PORT].asInt();
	}
	
	if(getParameter()[OPT_HTTP_THREAD_NUMBER].isNull() ||
       !getParameter()[OPT_HTTP_THREAD_NUMBER].isInt()) {
		thread_number = 1;
	} else {
		thread_number = getParameter()[OPT_HTTP_THREAD_NUMBER].asInt();
	}
	
    HTTWAN_INTERFACE_APP_ << "HTTP server listen on port: " << service_port;
    HTTWAN_INTERFACE_APP_ << "HTTP server thread used: " << thread_number;
    
	//allcoate each server for every thread
	for(int idx = 1;
		idx <= thread_number;
		idx++) {
		struct mg_server *http_server = mg_create_server(this);
		if(!http_server) continue;
		
		//configure server
		HTTWAN_INTERFACE_APP_ << " Thread " << idx << " allocated";
        std::string str_port = boost::lexical_cast<std::string>(service_port);
        mg_set_option(http_server, "listening_port", str_port.c_str());
		mg_set_option(http_server, "enable_keep_alive", "yes");
		mg_set_option(http_server, "enable_directory_listing", "false");
		HTTWAN_INTERFACE_APP_ << " Thread " << idx << " configured";
		//configure handler
		mg_add_uri_handler(http_server, "/api/", event_handler);
		mg_server_do_i_handle(http_server, do_i_handle);
		HTTWAN_INTERFACE_APP_ << " Thread " << idx << " attached to handler";
		//add server to the list
		http_server_list.push_back(http_server);
	}
	if(!http_server_list.size()) throw chaos::CException(-1, "No http server has been instantiated", __PRETTY_FUNCTION__);
	
	//construct the direct io service url
	service_url = boost::str( boost::format("http://<host>:%1%/rpc/domain/action[post data]") %  service_port);
}

//inherited method
void HTTPWANInterface::start() throw(CException) {
	
	run = true;
	thread_index = 0;
	for(ServerListIterator it = http_server_list.begin();
		it != http_server_list.end();
		it++) {
		http_server_thread.add_thread(new boost::thread(boost::bind(&HTTPWANInterface::pollHttpServer, this, *it)));
	}
}

//inherited method
void HTTPWANInterface::stop() throw(CException) {
	run = false;
	http_server_thread.join_all();
}

//inherited method
void HTTPWANInterface::deinit() throw(CException) {
	for(ServerListIterator it = http_server_list.begin();
		it != http_server_list.end();
		it++) {
		mg_destroy_server(&(*it));
	}
	http_server_list.clear();
	//clear the service url
	service_url.clear();
	service_port = 0;
}

void HTTPWANInterface::pollHttpServer(struct mg_server *http_server) {
	int current_index = ++thread_index;
	HTTWAN_INTERFACE_APP_ << "Entering http thread " << current_index;
	while (run) {
		mg_poll_server(http_server, 1000);
	}
	HTTWAN_INTERFACE_APP_ << "Leaving http thread " << current_index;
	
}

int HTTPWANInterface::process(struct mg_connection *connection) {
	CHAOS_ASSERT(handler)
	int								err = 0;
    DEBUG_CODE(uint64_t                        execution_time_start = TimingUtil::getTimeStampInMicrosends();)
    DEBUG_CODE(uint64_t                        execution_time_end = 0;)
	Json::Value						json_request;
	Json::Value						json_response;
	Json::StyledWriter				json_writer;
	Json::Reader					json_reader;
	HTTPWANInterfaceStringResponse	response("application/json");

	//scsan for content type request
	const std::string method  = connection->request_method;
	const std::string url     = connection->uri;
	const std::string api_uri = url.substr(strlen(API_PREFIX_V1)+1);
	const std::string content = mg_get_header(connection, "Content-Type");
	const bool        json    = content.compare("application/json") == 0;
	
	//remove the prefix and tokenize the url
	std::vector<std::string> api_token_list;
	algorithm::split(api_token_list,
					 api_uri,
					 algorithm::is_any_of("/"),
					 algorithm::token_compress_on);
	//check if we havethe domain and api name in the uri and the content is json
	if(api_token_list.size()>= 2 &&
	   json) {
		std::string content_data(connection->content, connection->content_len);
        if(json_reader.parse(content_data, json_request)) {
            //print the received JSON document
            DEBUG_CODE(HTTWAN_INTERFACE_DBG_ << "Received JSON pack:" <<json_writer.write(json_request);)

            //call the handler
            if((err = handler->handleCall(1,
                                          api_token_list,
                                          json_request,
                                          response.getHeader(),
                                          json_response))) {
                DEBUG_CODE(HTTWAN_INTERFACE_ERR_ << "Error on api call :" << connection->uri <<
                           (content_data.size()? (" with message data: " + content_data):" with no message data");)
                //return the error for the api call
                response.setCode(400);
                json_response["error"] = err;
                json_response["error_message"].append("Call Error");
            }else{
                //return the infromation of api call success
                response.setCode(200);
                json_response["error"] = 0;
            }
        }else{
            response.setCode(400);
            json_response["error"] = -1;
            json_response["error_message"].append("Error parsing the json post data");
            DEBUG_CODE(HTTWAN_INTERFACE_ERR_ << "Error decoding the request:" << json_writer.write(json_response);)
        }
		
		
	} else {
		//return the error for bad json or invalid url
		response.setCode(400);
		response.addHeaderKeyValue("Content-Type", "application/json");
		json_response["error"] = -1;
        if(api_token_list.size()<2) {
            json_response["error_message"].append("The uri need to contains either the domain and name of the api ex: http[s]://host:port/api/vx/domain/name(/param)*");
        }
        if(!json) {
            json_response["error_message"].append("The content of the request need to be json");
        }
        DEBUG_CODE(HTTWAN_INTERFACE_ERR_ << "Error decoding the request:" <<json_writer.write(json_response);)
	}
	
	response << json_writer.write(json_response);
	flush_response(connection, &response);
    DEBUG_CODE(execution_time_end = TimingUtil::getTimeStampInMicrosends();)
    DEBUG_CODE(uint64_t duration = execution_time_end - execution_time_start;)
    DEBUG_CODE(HTTWAN_INTERFACE_DBG_ << "Execution time is:" << duration << " microseconds";)
	return 1;//
}

bool HTTPWANInterface::handle(struct mg_connection *connection) {
    bool accepted = false;
    if(!(accepted = regex_match(connection->uri, REG_API_URL_FORMAT))) {
        HTTWAN_INTERFACE_ERR_ << "URI:" << connection->uri << ", not accepted";
    }
	return accepted;
}

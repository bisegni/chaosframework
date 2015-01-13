/*
 *	HTTPWANInterface.cpp
 *	!CHOAS
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

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include <json/json.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::wan_proxy::wan_interface;
using namespace chaos::wan_proxy::wan_interface::http;

#define HTTWANINTERFACE_LOG_HEAD "["<<getName()<<"] - "

#define HTTWAN_INTERFACE_APP_ LAPP_ << HTTWANINTERFACE_LOG_HEAD
#define HTTWAN_INTERFACE_DBG_ LDBG_ << HTTWANINTERFACE_LOG_HEAD << __PRETTY_FUNCTION__
#define HTTWAN_INTERFACE_ERR_ LERR_ << HTTWANINTERFACE_LOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ")"

static const boost::regex REG_API_URL_FORMAT("/api/v1((/[a-zA-Z0-9_]+))*");

static string htmlEntities(const string& data) {
	string buffer;
	buffer.reserve(data.size());
	
	for(size_t pos = 0; pos != data.size(); ++pos) {
		switch(data[pos]) {
			case '&':  buffer.append("&amp;");       break;
			case '\"': buffer.append("&quot;");      break;
			case '\'': buffer.append("&apos;");      break;
			case '<':  buffer.append("&lt;");        break;
			case '>':  buffer.append("&gt;");        break;
			default:   buffer.append(1, data[pos]); break;
		}
	}
	
	return buffer;
}


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
thread_number(0){
	
}

HTTPWANInterface::~HTTPWANInterface() {
	
}

//inherited method
void HTTPWANInterface::init(void *init_data) throw(CException) {
	//! forward message to superclass
	AbstractWANInterface::init(init_data);
	
	//clear in case last deinit fails
	http_server_list.clear();
	
	//check for parameter
	if(!getParameter().hasKey(OPT_HTTP_PORT)) {
		std::string err = "Port for http interface as not be set!";
		HTTWAN_INTERFACE_ERR_ << err;
		throw chaos::CException(-1, err, __PRETTY_FUNCTION__);
	} else {
		service_port = boost::lexical_cast<int>(getParameter().getStringValue(OPT_HTTP_PORT));
	}
	
	if(!getParameter().hasKey(OPT_HTTP_THREAD_NUMBER)) {
		thread_number = 1;
	} else {
		thread_number = boost::lexical_cast<int>(getParameter().getStringValue(OPT_HTTP_THREAD_NUMBER));
	}
	
	//allcoate each server for every thread
	for(int idx = 1;
		idx <= thread_number;
		idx++) {
		struct mg_server *http_server = mg_create_server(this);
		if(!http_server) continue;
		
		//configure server
		HTTWAN_INTERFACE_APP_ << " Thread " << idx << " allocated";
		mg_set_option(http_server, "listening_port", getParameter().getStringValue(OPT_HTTP_PORT).c_str());
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
	Json::Value json_answer;
	Json::StyledWriter writer;
	HTTPWANInterfaceStringResponse response;
	response.addHeaderKeyValue("Content-Type", "application/json");
	
	const std::string method  = connection->request_method;
	const std::string url     = connection->uri;
	const std::string content = mg_get_header(connection, "Content-Type");
	const bool        json    = content.compare("application/json") == 0;
	const bool        bson    = json?false:content.compare("application/bson") == 0;
	
	std::vector<std::string> api_parameter_in_url;
	algorithm::split(api_parameter_in_url,
					 url,
					 algorithm::is_any_of("/"),
					 algorithm::token_compress_on);
	
	if(api_parameter_in_url.size() > 3 &&
	   (json || bson)) {
		CDataWrapper *message_data = NULL;
		std::string content_data(connection->content, connection->content_len);
		
		if (json) {
			message_data = new CDataWrapper();
			message_data->setSerializedJsonData(content_data.c_str());
		} else if(bson) {
			message_data = new CDataWrapper(content_data.c_str());
		}
		
		DEBUG_CODE(HTTWAN_INTERFACE_DBG_ << "Call api fordomain:" << api_parameter_in_url[2]
				   << " action:" << api_parameter_in_url[3] << " and message data:"
				   << (message_data!=NULL?message_data->getJSONString():"No message data");)
		
		
		//write response
		response.setCode(200);
		
		json_answer["Error"] = "Call Succeded";
		json_answer["input_message"] = message_data->getJSONString();
	} else {
		response.setCode(400);
		response.addHeaderKeyValue("Content-Type", "application/json");
		json_answer["Error"] = "Invalid uri";
	}
	
	response << writer.write(json_answer);
	flush_response(connection, &response);
	return 1;//
}

bool HTTPWANInterface::handle(struct mg_connection *connection) {
	//connection->request_method, connection->uri
	return regex_match(connection->uri, REG_API_URL_FORMAT);
}

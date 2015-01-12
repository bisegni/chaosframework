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

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::wan_proxy::wan_interface;

#define HTTWANINTERFACE_LOG_HEAD "["<<getName()<<"] - "

#define HTTWAN_INTERFACE_APP_ LAPP_ << HTTWANINTERFACE_LOG_HEAD
#define HTTWAN_INTERFACE_DBG_ LDBG_ << HTTWANINTERFACE_LOG_HEAD << __PRETTY_FUNCTION__
#define HTTWAN_INTERFACE_ERR_ LERR_ << HTTWANINTERFACE_LOG_HEAD << __PRETTY_FUNCTION__ << "(" << __LINE__ << ")"

static const boost::regex REG_API_URL_FORMAT("/api/v1((/[a-zA-Z0-9_]+))*");

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
						   HTTPWANInterfaceResponse *response) {
	CHAOS_ASSERT(connection && response)
	mg_send_status(connection, response->getCode());
	
	for(std::map<std::string, std::string>::const_iterator it = response->getHeader().begin();
		it != response->getHeader().end();
		it++){
		mg_send_header(connection, it->first.c_str(), it->second.c_str());
	}
	
	
	
	//mg_send_data(struct mg_connection *, const void *data, int data_len);
}

DEFINE_CLASS_FACTORY(HTTPWANInterface, AbstractWANInterface);
HTTPWANInterface::HTTPWANInterface(const string& alias):
AbstractWANInterface(alias),
http_server(NULL){
	
}

HTTPWANInterface::~HTTPWANInterface() {
	
}

//inherited method
void HTTPWANInterface::init(void *init_data) throw(CException) {
	//! forward message to superclass
	AbstractWANInterface::init(init_data);
	
	//check for parameter
	if(!getParameter().hasKey(OPT_HTTP_PORT)) {
		std::string err = "Port for http interface as not be set!";
		HTTWAN_INTERFACE_ERR_ << err;
		throw chaos::CException(-1, err, __PRETTY_FUNCTION__);
	}
	//fetch the port
	service_port = boost::lexical_cast<int>(getParameter().getStringValue(OPT_HTTP_PORT));
	http_server = mg_create_server(this);
	if(!http_server) throw chaos::CException(-1, "Http server not instantiated", __PRETTY_FUNCTION__);
	
	mg_set_option(http_server, "listening_port", getParameter().getStringValue(OPT_HTTP_PORT).c_str());
	mg_set_option(http_server, "enable_keep_alive", "yes");
	mg_set_option(http_server, "enable_directory_listing", "false");
	
	mg_add_uri_handler(http_server, "/api/", event_handler);
	mg_server_do_i_handle(http_server, do_i_handle);
	
	//construct the direct io service url
	service_url = boost::str( boost::format("http://<host>:%1%/rpc/domain/action[post data]") %  service_port);
	
}

//inherited method
void HTTPWANInterface::start() throw(CException) {
	CHAOS_ASSERT(http_server)
	run = true;
	thread_index = 0;
	http_server_thread.add_thread(new boost::thread(boost::bind(&HTTPWANInterface::pollHttpServer, this, http_server)));
}

//inherited method
void HTTPWANInterface::stop() throw(CException) {
	CHAOS_ASSERT(http_server)
	run = false;
	http_server_thread.join_all();
}

//inherited method
void HTTPWANInterface::deinit() throw(CException) {
	if(http_server){
		mg_destroy_server(&http_server);
		http_server = NULL;
	}
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



/*
Response *HTTPWANInterface::process(Request &request) {
	StreamResponse *response = new StreamResponse();
	std::string method  = request.getMethod();
	std::string url     = request.getUrl();
	std::string content = request.getHeaderKeyValue("Content-Type");
	bool        json    = content.compare("application/json") == 0;
	bool        bson    = json?false:content.compare("application/bson") == 0;
	
	std::vector<std::string> api_parameter_in_url;
	algorithm::split(api_parameter_in_url,
					 url,
					 algorithm::is_any_of("/"),
					 algorithm::token_compress_on);
	
	if(api_parameter_in_url.size()== 4 &&
	   (json || bson)) {
		CDataWrapper *message_data = NULL;
		std::string data  = request.getData();
		
		if (json) {
			message_data = new CDataWrapper();
			message_data->setSerializedJsonData(data.c_str());
		} else if(bson) {
			message_data = new CDataWrapper(data.c_str());
		}
		
		DEBUG_CODE(HTTWAN_INTERFACE_DBG_ << "Call api fordomain:" << api_parameter_in_url[2]
				   << " action:" << api_parameter_in_url[3] << " and message data:"
				   << (message_data!=NULL?message_data->getJSONString():"No message data");)
		
		
		//write response
		response->setCode(200);
		//if(result.get()) {
		//	if (json) {
		//		result_buffer = result->getJSONData();
		//		response->setHeader("Content-Type", "application/json");
		//	} else if(bson) {
		//		result_buffer = result->getBSONData();
		//		response->setHeader("Content-Type", "application/bson");
		//	}
		//	response->write(result_buffer->getBufferPtr(), result_buffer->getBufferLen());
		//}
	//} else {
		response->setCode(400);
		response->setHeader("Content-Type", "application/txt");
		*response << htmlEntities("Invalid api call!") << endl;
	}
	return response;
}*/

int HTTPWANInterface::process(struct mg_connection *connection) {
	mg_printf_data(connection, "Hello! Requested URI is [%s]", connection->uri);
	return 1;//
}

bool HTTPWANInterface::handle(struct mg_connection *connection) {
	//connection->request_method, connection->uri
	return regex_match(connection->uri, REG_API_URL_FORMAT);
}

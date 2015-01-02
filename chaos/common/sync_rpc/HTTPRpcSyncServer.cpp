/*
 *	HTTPRpcSyncServer.cpp
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

#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/sync_rpc/HTTPRpcSyncServer.h>
#include <chaos/common/data/CDataWrapper.h>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::sync_rpc;
using namespace Mongoose;

#define HTTPRSS_LOG_HEAD "["<<getName()<<"] - "

#define HTTPRSSLAPP_ LAPP_ << HTTPRSS_LOG_HEAD
#define HTTPRSSLDBG_ LDBG_ << HTTPRSS_LOG_HEAD << __FUNCTION__
#define HTTPRSSLERR_ LERR_ << HTTPRSS_LOG_HEAD

static const boost::regex REG_API_URL_FORMAT("/rpc((/[a-zA-Z0-9_]+))*");

HTTPRpcSyncServer::HTTPRpcSyncServer(const string& alias):
RpcSyncServer(alias),
http_server(8080){
	
}

HTTPRpcSyncServer::~HTTPRpcSyncServer() {
	
}

//inherited method
void HTTPRpcSyncServer::init(void*) throw(CException) {
    http_server.registerController(this);
    http_server.setOption("enable_directory_listing", "false");
    http_server.setOption("enable_keep_alive", "yes");
    const std::map<std::string, std::string>& kv_param = GlobalConfiguration::getInstance()->getRpcImplKVParam();
    if(kv_param.size()) {
        //we have some kv entry
        for (std::map<std::string, std::string>::const_iterator it = kv_param.begin();
             it != kv_param.end();
             it++) {
            HTTPRSSLAPP_ << "Set custom key value param "<<it->first << "=" <<it->second;
            //add all option if the are recognized
            if(it->first.compare("listening_ports") == 0 ||
               it->first.compare("ssl_certificate") == 0 ||
               it->first.compare("num_threads") == 0) {
                http_server.setOption(it->first.c_str(), it->second.c_str());
            } else {
                HTTPRSSLAPP_ << it->first << " is an invalid key";  
            }
        }
    }
}

//inherited method
void HTTPRpcSyncServer::start() throw(CException) {
    http_server.start();
    cout << "Server started, routes:" << endl;
}

//inherited method
void HTTPRpcSyncServer::stop() throw(CException) {
    http_server.stop();
}

//inherited method
void HTTPRpcSyncServer::deinit() throw(CException) {
	
}

Response *HTTPRpcSyncServer::process(Request &request) {
    SerializationBuffer *result_buffer = NULL;
    StreamResponse *response = new StreamResponse();
    std::string method  = request.getMethod();
    std::string url     = request.getUrl();
    std::string content = request.getHeaderKeyValue("Content-Type");
    bool        json    = content.compare("application/json") == 0;
    bool        bson    = json?false:content.compare("application/bson") == 0;
    HTTPRSSLAPP_ << method;
    HTTPRSSLAPP_ << url;
    HTTPRSSLAPP_ << content;
    HTTPRSSLAPP_ << request.getData();

    std::vector<std::string> api_parameter_in_url;
    algorithm::split(api_parameter_in_url,
                     url,
                     algorithm::is_any_of("/"),
                     algorithm::token_compress_on);
    
    for(std::vector<std::string>::iterator it = api_parameter_in_url.begin();
        it != api_parameter_in_url.end();
        it++) {
        HTTPRSSLAPP_ << *it;
    }
    
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
        
        DEBUG_CODE(HTTPRSSLDBG_ << "Call api fordomain:" << api_parameter_in_url[2]
                                << " action:" << api_parameter_in_url[3] << " and message data:"
                                << (message_data!=NULL?message_data->getJSONString():"No message data");)
        std::auto_ptr<CDataWrapper> result(commandHandler->executeCommandSync(api_parameter_in_url[2],
                                                                              api_parameter_in_url[3],
                                                                              message_data));
        if (json) {
            result_buffer = result->getJSONData();
            response->setHeader("Content-Type", "application/json");
        } else if(bson) {
            result_buffer = result->getBSONData();
            response->setHeader("Content-Type", "application/bson");
        }
        //write response
        response->setCode(200);
        response->write(result_buffer->getBufferPtr(), result_buffer->getBufferLen());
    } else {
        response->setCode(400);
        response->setHeader("Content-Type", "application/bson");
        *response << htmlEntities("Invalid api call!") << endl;
    }
    return response;
}

bool HTTPRpcSyncServer::handles(string method, string url) {
    return regex_match(url, REG_API_URL_FORMAT);
}
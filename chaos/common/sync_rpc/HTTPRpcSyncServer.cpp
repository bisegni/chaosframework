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
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::sync_rpc;
using namespace Mongoose;

#define HTTPRSS_LOG_HEAD "["<<getName()<<"] - "

#define HTTPRSSLAPP_ LAPP_ << HTTPRSS_LOG_HEAD
#define HTTPRSSLDBG_ LDBG_ << HTTPRSS_LOG_HEAD << __FUNCTION__
#define HTTPRSSLERR_ LERR_ << HTTPRSS_LOG_HEAD

static const boost::regex REG_API_URL_FORMAT("/rpc((/[a-zA-Z0-9_]+))*");

DEFINE_CLASS_FACTORY(HTTPRpcSyncServer, RpcSyncServer);
HTTPRpcSyncServer::HTTPRpcSyncServer(const string& alias):
RpcSyncServer(alias),
http_server(NULL){
	
}

HTTPRpcSyncServer::~HTTPRpcSyncServer() {
	
}

//inherited method
void HTTPRpcSyncServer::init(void *init_data) throw(CException) {
	CDataWrapper *adapterConfiguration = reinterpret_cast<CDataWrapper*>(init_data);
	service_port = adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_SYNC_ADAPTER_PORT);
	http_server = new Mongoose::Server(service_port);
	if(!http_server) throw chaos::CException(-1, "Http server not instantiated", __PRETTY_FUNCTION__);
	
    http_server->registerController(this);
    http_server->setOption("enable_directory_listing", "false");
    http_server->setOption("enable_keep_alive", "yes");
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
                http_server->setOption(it->first.c_str(), it->second.c_str());
            } else {
                HTTPRSSLAPP_ << it->first << " is an invalid key";  
            }
        }
    }
	
	//construct the direct io service url
	service_url = boost::str( boost::format("http://%1%:%2%/rpc/domain/action[post data]") % adapterConfiguration->getStringValue("local_ip") % service_port);

}

//inherited method
void HTTPRpcSyncServer::start() throw(CException) {
	CHAOS_ASSERT(http_server)
    http_server->start();
}

//inherited method
void HTTPRpcSyncServer::stop() throw(CException) {
	CHAOS_ASSERT(http_server)
    http_server->stop();
}

//inherited method
void HTTPRpcSyncServer::deinit() throw(CException) {
	if(http_server){DELETE_OBJ_POINTER(http_server)}
	//clear the service url
	service_url.clear();
	service_port = 0;
}

Response *HTTPRpcSyncServer::process(Request &request) {
    SerializationBuffer *result_buffer = NULL;
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
        
        DEBUG_CODE(HTTPRSSLDBG_ << "Call api fordomain:" << api_parameter_in_url[2]
                                << " action:" << api_parameter_in_url[3] << " and message data:"
                                << (message_data!=NULL?message_data->getJSONString():"No message data");)
        std::auto_ptr<CDataWrapper> result(commandHandler->executeCommandSync(api_parameter_in_url[2],
                                                                              api_parameter_in_url[3],
                                                                              message_data));
		
		
		//write response
		response->setCode(200);
		if(result.get()) {
			if (json) {
				result_buffer = result->getJSONData();
				response->setHeader("Content-Type", "application/json");
			} else if(bson) {
				result_buffer = result->getBSONData();
				response->setHeader("Content-Type", "application/bson");
			}
			
			//if we have houtput...send it
			if(result_buffer) {
				response->write(result_buffer->getBufferPtr(), result_buffer->getBufferLen());
				DELETE_OBJ_POINTER(result_buffer)
			}
		}
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

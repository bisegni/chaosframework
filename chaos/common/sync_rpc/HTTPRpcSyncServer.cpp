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
#include <chaos/common/sync_rpc/HTTPRpcSyncServer.h>


using namespace chaos::common::sync_rpc;
using namespace Mongoose;

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
}

//inherited method
void HTTPRpcSyncServer::start() throw(CException) {
    
    http_server.start();
    
    cout << "Server started, routes:" << endl;
    dumpRoutes();

}

//inherited method
void HTTPRpcSyncServer::stop() throw(CException) {
    http_server.stop();
}

//inherited method
void HTTPRpcSyncServer::deinit() throw(CException) {
	
}

void HTTPRpcSyncServer::consumeJSONApi(Request &request, StreamResponse &response) {
    LAPP_ << request.getUrl();
    response << "Hello " << htmlEntities(request.get("name", "... what's your name ?")) << endl;
}

void HTTPRpcSyncServer::setup() {
    // Example of prefix, putting all the urls into "/api"
    WebController::setPrefix("/api");
    
    // Hello demo
    WebController::addRoute("GET", "/devices", HTTPRpcSyncServer, consumeJSONApi);
    WebController::addRoute("GET", "/device/dataset", HTTPRpcSyncServer, consumeJSONApi);
}
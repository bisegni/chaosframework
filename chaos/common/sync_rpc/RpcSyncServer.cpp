/*
 *	RpcSyncServer.cpp
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

#include <chaos/common/sync_rpc/RpcSyncServer.h>

#include <boost/format.hpp>
using namespace chaos::common::sync_rpc;

RpcSyncServer::RpcSyncServer(const std::string& alias):
NamedService(alias) {}

RpcSyncServer::~RpcSyncServer(){}

/*!
 Return the published port
 */
int RpcSyncServer::getPublishedPort(){
	return service_port;
}

//! inherited method
const std::string& RpcSyncServer::getUrl() {
	return service_url;
}
/*
 set the command dispatcher associated to the instance of rpc adapter
 */
void RpcSyncServer::setCommandDispatcher(chaos::RpcServerHandler *newCommandHandler) {
	commandHandler = newCommandHandler;
}

/*
 init the rpc adapter
 */
void RpcSyncServer::init(void*) throw(CException) {
}

/*
 start the rpc adapter
 */
void RpcSyncServer::start() throw(CException) {
	
}

/*
 start the rpc adapter
 */
void RpcSyncServer::stop() throw(CException) {
	
}

/*
 deinit the rpc adapter
 */
void RpcSyncServer::deinit() throw(CException) {
	
}

/*
 *	ApiServer.cpp
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

#include "ApiServer.h"
#include <chaos/common/global.h>

using namespace chaos;
using namespace chaos::common::data;

using namespace chaos::cnd;
using namespace chaos::cnd::api;

#define AS_LAPP_ LAPP_ << "[ApiServer] - "

/*!
 */
ApiServer::ApiServer() {
	
}

/*!
 */
ApiServer::~ApiServer() {
}

// Initialize instance
void ApiServer::init(void *initData) throw(chaos::CException) {
    AS_LAPP_ << "Init";
    AS_LAPP_ << "Init NetworkBroker";
    networkBroker->init();
    AS_LAPP_ << "NetworkBroker Initialized";

}

// Start the implementation
void ApiServer::start() throw(chaos::CException) {
    AS_LAPP_ << "Starting DataManagment";
    networkBroker->start();
    AS_LAPP_ << "NetworkBroker Started";

}

// Start the implementation
void ApiServer::stop() throw(chaos::CException) {
    AS_LAPP_ << "Starting NetworkBroker";
    networkBroker->stop();
    AS_LAPP_ << "NetworkBroker Started";
}

// Deinit the implementation
void ApiServer::deinit() throw(chaos::CException) {
    AS_LAPP_ << "Starting NetworkBroker";
    networkBroker->deinit();
    AS_LAPP_ << "NetworkBroker Started";
}
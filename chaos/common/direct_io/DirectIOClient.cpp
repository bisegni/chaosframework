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

#include <chaos/common/direct_io/DirectIOClient.h>

using namespace chaos::common::direct_io;

DirectIOClient::DirectIOClient() {
	
}

DirectIOClient::~DirectIOClient() {
	
}

// Initialize instance
void DirectIOClient::init(void *init_data) throw(chaos::CException) {
	if(!init_data) throw chaos::CException(0, "Invalid parameter", "DirectIOClient::init");
	connection_info.reset(static_cast<DirectIOConnection*>(init_data));

}

// Start the implementation
void DirectIOClient::start() throw(chaos::CException) {
	
}


// Stop the implementation
void DirectIOClient::stop() throw(chaos::CException) {
	
}


// Deinit the implementation
void DirectIOClient::deinit() throw(chaos::CException) {
	
}


//! Send some data to the server
int DirectIOClient::sendData(void *data, uint32_t len) {
	return 0;
}

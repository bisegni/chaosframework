/*
 *	AbstractDirectIODispatcher.cpp
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
#include <chaos/common/direct_io/DirectIODispatcher.h>
using namespace chaos::common::direct_io;

void setDelegate(DirectIOServerEndpoint *);

// Initialize instance
void DirectIODispatcher::init(void *init_data) throw(chaos::CException) {
	//allocate memory for the endpoint array
	endpoint_hash = (EndpointFastDelegation**)malloc(sizeof(EndpointFastDelegation**));
}

// Start the implementation
void DirectIODispatcher::start() throw(chaos::CException) {
	
}

// Stop the implementation
void DirectIODispatcher::stop() throw(chaos::CException) {
	
}

// Deinit the implementation
void DirectIODispatcher::deinit() throw(chaos::CException) {
	if(endpoint_hash) free(endpoint_hash);
}

DirectIOServerEndpoint *DirectIODispatcher::getNewEndpoint() {
	EndpointFastDelegation *new_fast_delegation = new EndpointFastDelegation();
	new_fast_delegation->endpoint = new DirectIOServerEndpoint();
	new_fast_delegation->priority_delegate = boost::bind(&DirectIOServerEndpoint::priorityDataReceived, new_fast_delegation->endpoint, _1);
	new_fast_delegation->service_delegate = boost::bind(&DirectIOServerEndpoint::serviceDataReceived, new_fast_delegation->endpoint, _1);
	return new_fast_delegation->endpoint;
}

// Event for a new data received
void DirectIODispatcher::priorityDataReceived(DirectIOServerDataPack *) {
	//get route index and call delegation
}

// Event for a new data received
void DirectIODispatcher::serviceDataReceived(DirectIOServerDataPack *) {
	
}
//
//  DirectIOServer.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 25/09/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/direct_io/DirectIOServer.h>

using namespace chaos::common::direct_io;

DirectIOServer::DirectIOServer(std::string alias):NamedService(alias) {
	
}

DirectIOServer::~DirectIOServer() {
	
}

// Initialize instance
void DirectIOServer::init(void *init_data) throw(chaos::CException) {
	if(handler_impl == NULL) throw chaos::CException(-1, "handler has not been configured", __FUNCTION__);
        
}

// Start the implementation
void DirectIOServer::start() throw(chaos::CException) {
	
}

// Stop the implementation
void DirectIOServer::stop() throw(chaos::CException) {
	
}

// Deinit the implementation
void DirectIOServer::deinit() throw(chaos::CException) {
	
}

//! Send some data to the server
void DirectIOServer::setHandler(DirectIOHandler *_handler_impl) {
	handler_impl = _handler_impl;
}

//! Remove the handler pointer
void DirectIOServer::clearHandler() {
	
}
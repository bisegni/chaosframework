//
//  AnswerEngine.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "AnswerEngine.h"

using namespace chaos::data_service;
using namespace chaos::common::direct_io;

AnswerEngine::AnswerEngine() {
	
}

AnswerEngine::~AnswerEngine() {
	
}

int AnswerEngine::registerNewClient(uint32_t client_hash, std::string client_address) {
	utility::InizializableServiceContainer<DirectIOClient> *new_client = ALLOCATE_IS_CONTAINER_WI(DirectIOClient, network_broker->getPointer()->getDirectIOClientInstance(), true);
	if(!new_client) return false;
	
	new_client->init(NULL, __PRETTY_FUNCTION__);
	new_client->getPointer()->setConnectionMode(DirectIOConnectionSpreadType::DirectIORoundRobin);
	new_client->getPointer()->addServer(client_address);
	
	map_client.insert(make_pair(client_hash, new_client));
	return 0;
}

void AnswerEngine::sendCacheAnswher(uint32_t client_hash, void *answer) {
	
}

//! Initialize instance
void AnswerEngine::init(void *init_data) throw(chaos::CException) {
	
}

//! Deinit the implementation
void AnswerEngine::deinit() throw(chaos::CException) {
	
}
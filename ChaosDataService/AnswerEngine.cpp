//
//  AnswerEngine.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include "AnswerEngine.h"
#include <chaos/common/utility/InetUtility.h>

using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;
#define AnswerEngine_LOG_HEAD "[AnswerEngine] - "

#define AELAPP_ LAPP_ << AnswerEngine_LOG_HEAD
#define AELDBG_ LDBG_ << AnswerEngine_LOG_HEAD
#define AELERR_ LERR_ << AnswerEngine_LOG_HEAD

AnswerEngine::AnswerEngine():network_broker(NULL) {
	
}

AnswerEngine::~AnswerEngine() {
	
}

int AnswerEngine::registerNewClient(opcode_headers::DirectIODeviceChannelHeaderGetOpcode& client_header) {
	CHAOS_ASSERT(InizializableService::getServiceState() == chaos::utility::service_state_machine::InizializableServiceType::IS_INITIATED)
	
    if(map_client.count(client_header.field.device_hash)) {
        //client already registered
        return 0;
    }
    
	boost::shared_ptr< DirectIOClient > new_client( network_broker->getDirectIOClientInstance() );
	if(!new_client) return -2;

        //initialize the client implementation
	chaos::utility::InizializableService::initImplementation(new_client.get(), NULL, "DirectIOClient", __PRETTY_FUNCTION__);
        //set conenction type for the client
	new_client->setConnectionMode(DirectIOConnectionSpreadType::DirectIORoundRobin);
        //add the request client address
	new_client->addServer(UI64_TO_STRIP(client_header.field.address));
	
        //allcoate the client
	DirectIODeviceClientChannel *device_channel = (DirectIODeviceClientChannel*)new_client->getNewChannelInstance("DirectIODeviceClientChannel");
	if(!device_channel) {
        chaos::utility::InizializableService::deinitImplementation(new_client.get(),  "DirectIOClient", __PRETTY_FUNCTION__);
		return -3;
	}
        //all is gone well
        //now we can add client and channel to the maps
	map_channel.insert(make_pair(client_header.field.device_hash, device_channel));
	map_client.insert(make_pair(client_header.field.device_hash, new_client));
	return 0;
}

void AnswerEngine::sendCacheAnswher(uint32_t client_hash, void *buffer, uint32_t buffer_len) {
	//send answer to client
	map_channel[client_hash]->putDataOutputChannel(true, buffer, buffer_len);
}

//! Initialize instance
void AnswerEngine::init(void *init_data) throw(chaos::CException) {
	AELAPP_ << "check for network broker instance";
	if(!network_broker) throw CException(-1, "No network brocker associated", "AnswerEngine::init");
}

//! Deinit the implementation
void AnswerEngine::deinit() throw(chaos::CException) {
	AELAPP_ << "delete all client instance";
	for (MapClientIterator iter = map_client.begin(); iter != map_client.end(); iter++) {
		
		
		if(map_channel.count(iter->first)) {
			AELAPP_ << "release channel for client hash " << iter->first;
			iter->second->releaseChannelInstance(map_channel[iter->first]);
			
			AELAPP_ << "Erase channel map entry for hash" << iter->first;
			map_channel.erase(iter->first);
		}
	
		AELAPP_ << "Deinitializing client map entry for hash" << iter->first;
		try {
			chaos::utility::InizializableService::deinitImplementation(iter->second.get(), "DirectIOClient", __PRETTY_FUNCTION__);
		} catch(chaos::CException& exc) {
			
		} catch(...) {
			
		}
	}
	AELAPP_ << "Delete all client map entry";
	map_client.clear();
}
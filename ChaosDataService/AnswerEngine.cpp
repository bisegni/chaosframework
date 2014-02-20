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

#define AnswerEngine_LOG_HEAD "[AnswerEngine] - "

#define AELAPP_ LAPP_ << AnswerEngine_LOG_HEAD
#define AELDBG_ LDBG_ << AnswerEngine_LOG_HEAD
#define AELERR_ LERR_ << AnswerEngine_LOG_HEAD

AnswerEngine::AnswerEngine():network_broker(NULL) {
	
}

AnswerEngine::~AnswerEngine() {
	
}

int AnswerEngine::registerNewClient(uint32_t client_hash, std::string client_address) {
	if(InizializableService::getServiceState() == chaos::utility::service_state_machine::InizializableServiceType::IS_DEINTIATED)
		return -1;
	
	boost::shared_ptr< DirectIOClient > new_client( network_broker->getDirectIOClientInstance() );
	if(!new_client) return -2;

	chaos::utility::InizializableService::initImplementation(new_client.get(), NULL, "DirectIOClient", __PRETTY_FUNCTION__);

	new_client->setConnectionMode(DirectIOConnectionSpreadType::DirectIORoundRobin);
	new_client->addServer(client_address);
	
	
	DirectIODeviceClientChannel *device_channel = (DirectIODeviceClientChannel*)new_client->getNewChannelInstance("DirectIODeviceClientChannel");
	if(!device_channel) {
		return -3;
	}
	
	//now we can add client and channel to the maps
	map_channel.insert(make_pair(client_hash, device_channel));
	map_client.insert(make_pair(client_hash, new_client));
	return 0;
}

void AnswerEngine::sendCacheAnswher(uint32_t client_hash, void *answer) {
	//send answer to client
	map_channel[client_hash]->sendWithPriority(DeviceChannelOpcode::DeviceChannelOpcodePutInputWithCache, static_cast<chaos::common::data::SerializationBuffer*>(answer));
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
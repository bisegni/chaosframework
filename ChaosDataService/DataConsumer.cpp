/*
 *	DataConsumer.h
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

#include "DataConsumer.h"

#include <chaos/common/utility/ObjectFactoryRegister.h>

using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define DataConsumer_LOG_HEAD "[DataConsumer] - "

#define DSLAPP_ LAPP_ << DataConsumer_LOG_HEAD
#define DSLDBG_ LDBG_ << DataConsumer_LOG_HEAD
#define DSLERR_ LERR_ << DataConsumer_LOG_HEAD

DataConsumer::DataConsumer(){
    
}

DataConsumer::~DataConsumer() {
    
}

void DataConsumer::init(void *init_data) throw (chaos::CException) {
	if(!server_endpoint) throw chaos::CException(-1, "Invalid server endpoint", __FUNCTION__);
	DSLAPP_ << "DataCOnsumer initialized with endpoint "<< server_endpoint->getRouteIndex();
	
	DSLAPP_ << "Allocating DirectIOCDataWrapperServerChannel";
	server_channel = (DirectIOCDataWrapperServerChannel*)chaos::ObjectFactoryRegister<DirectIOVirtualServerChannel>::getInstance()->getNewInstanceByName("DirectIOCDataWrapperServerChannel");
	if(!server_channel) throw chaos::CException(-2, "Error allocating rpc server channel", __FUNCTION__);
	
	DSLAPP_ << "Register DirectIOCDataWrapperServerChannel into the endpoint";
	server_endpoint->registerChannelInstance(server_channel);
	
	server_channel->setHandler(this);
}

void DataConsumer::start() throw (chaos::CException) {
    
}

void DataConsumer::stop() throw (chaos::CException) {
    
}

void DataConsumer::deinit() throw (chaos::CException) {
	DSLAPP_ << "Deregister DirectIOCDataWrapperServerChannel into the endpoint";
	server_endpoint->deregisterChannelInstance(server_channel);
	
	DSLAPP_ << "Delete DirectIOCDataWrapperServerChannel";
    if(server_channel) delete(server_channel);
}


void DataConsumer::consumeCDataWrapper(uint8_t channel_tag, chaos::common::data::CDataWrapper *data_wrapper) {
    
}

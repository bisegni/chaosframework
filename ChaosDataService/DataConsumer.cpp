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
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/channel/DirectIOCDataWrapperClientChannel.h>
#include <boost/thread.hpp>

using namespace chaos::data_service;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;

#define DataConsumer_LOG_HEAD "[DataConsumer] - "

#define DSLAPP_ LAPP_ << DataConsumer_LOG_HEAD
#define DSLDBG_ LDBG_ << DataConsumer_LOG_HEAD
#define DSLERR_ LERR_ << DataConsumer_LOG_HEAD

bool work = false;

DataConsumer::DataConsumer():sent(0), received(0), last_sent(0), last_received(0) {
    
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
	
	last_received_ts = last_sent_ts = timing_util.getTimeStamp();
}

void DataConsumer::start() throw (chaos::CException) {
    DSLAPP_ << "Start Data Consumer";
    work = true;
}

void DataConsumer::stop() throw (chaos::CException) {
    DSLAPP_ << "Stop Data Consumer";
    work = false;
    client_threads_group.join_all();
}

void DataConsumer::deinit() throw (chaos::CException) {
	DSLAPP_ << "Deregister DirectIOCDataWrapperServerChannel into the endpoint";
	server_endpoint->deregisterChannelInstance(server_channel);
	
	DSLAPP_ << "Delete DirectIOCDataWrapperServerChannel";
    if(server_channel) delete(server_channel);
}


void DataConsumer::consumeCDataWrapper(uint8_t channel_tag, chaos::common::data::CDataWrapper *data_wrapper) {
	received++;
	if((received % 2000) == 0) {
		uint64_t time_spent = timing_util.getTimeStamp()-last_received_ts;
		DSLAPP_ << "received "<< (received - last_received) << " in " << time_spent << " msec";
		last_received = received;
		last_received_ts = timing_util.getTimeStamp();
	}
}

void DataConsumer::simulateClient(DirectIOClient *client_instance) {
    DSLAPP_ << "Entering client thread";
	DirectIOCDataWrapperClientChannel *channel = (DirectIOCDataWrapperClientChannel*)chaos::ObjectFactoryRegister<DirectIOVirtualClientChannel>::getInstance()->getNewInstanceByName("DirectIOCDataWrapperClientChannel");
    DSLAPP_ << "registering client";
    client_instance->registerChannelInstance(channel);
	while(work) {
		chaos::common::data::CDataWrapper data;
		data.addInt32Value("int_val", sent++);
		data.addStringValue("string_val", "str data");
		
		chaos::common::data::SerializationBuffer *s = data.getBSONData();
		
		channel->pushCDataWrapperSerializationBuffer(0, (uint8_t)1, s);
		delete(s);
		
		if((sent % 2000) == 0) {
			uint64_t time_spent = timing_util.getTimeStamp()-last_sent_ts;
			DSLAPP_ << "sent "<< (sent - last_sent) << " in " << time_spent << " msec";
			last_sent = sent;
			last_sent_ts = timing_util.getTimeStamp();
		}
	}
    DSLAPP_ << "deregistering client";
	client_instance->deregisterChannelInstance(channel);
}

void DataConsumer::addClient(DirectIOClient *client) {
	client_threads_group.add_thread(new thread(boost::bind(&DataConsumer::simulateClient, this, client)));
}
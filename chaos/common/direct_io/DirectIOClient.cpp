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

#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>

#include <boost/lexical_cast.hpp>

using namespace chaos::common::direct_io;
namespace chaos_data = chaos::common::data;

namespace b_algo = boost::algorithm;

#define DIO_LOG_HEAD "["<<getName()<<"] - "

#define DIOLAPP_ LAPP_ << DIO_LOG_HEAD
#define DIOLDBG_ LDBG_ << DIO_LOG_HEAD
#define DIOLERR_ LERR_ << DIO_LOG_HEAD

DirectIOClient::DirectIOClient(string alias):NamedService(alias) {
	
}

DirectIOClient::~DirectIOClient() {
    clearChannelInstancerAndInstance();
}

// Initialize instance
void DirectIOClient::init(void *init_data) throw(chaos::CException) {
}

// Start the implementation
void DirectIOClient::start() throw(chaos::CException) {
	
}

// Stop the implementation
void DirectIOClient::stop() throw(chaos::CException) {
	
}

// Deinit the implementation
void DirectIOClient::deinit() throw(chaos::CException) {
	clearChannelInstancerAndInstance();
}

void DirectIOClient::clearChannelInstancerAndInstance() {
}

// allocate a new channel by the instancer
channel::DirectIOVirtualClientChannel *DirectIOClient::registerChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance) {
    boost::unique_lock<boost::shared_mutex>	Lock(mutex_channel_map);
    if(channel_instance == NULL) return NULL;
    
    //associate the instance
    channel_instance->client_instance = this;
    channel_map.insert(make_pair(channel_instance->channel_route_index, channel_instance));
    return channel_instance;
}

// dispose the channel instance
void DirectIOClient::deregisterChannelInstance(channel::DirectIOVirtualClientChannel *channel_instance) {
    if(channel_instance == NULL) return;
    boost::unique_lock<boost::shared_mutex>	Lock(mutex_channel_map);
    channel_map.erase(channel_instance->channel_route_index);
    delete channel_instance;
}

//! Initialize instance
void DirectIOClient::updateConfiguration(void *init_data) throw(chaos::CException) {
    chaos_data::CDataWrapper *init_cdw =static_cast<chaos_data::CDataWrapper*>(init_data);
    if(init_cdw) return;
    
    DIOLAPP_ << "Receive new update for configuration";
    if(init_cdw->hasKey(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS)){
        DIOLAPP_ << "New servers configration has been provided";
        ServerFeeder::clear();
        DIOLAPP_ << "Old configuration has been removed";

        auto_ptr<chaos_data::CMultiTypeDataArrayWrapper> data_proxy_server_address(init_cdw->getVectorValue(LiveHistoryMDSConfiguration::CS_DM_LD_SERVER_ADDRESS));
        //update the live data address
        DIOLAPP_ << "Scan "<< data_proxy_server_address->size() << " server descriptions";
        for ( int idx = 0; idx < data_proxy_server_address->size(); idx++ ){
            vector<string> server_tokens;
            string server_desc = data_proxy_server_address->getStringElementAtIndex(idx);
            if(!ServerFeeder::addServer(server_desc)) {
                DIOLAPP_ << "Wrong Server Description '" << server_desc << "'";
            }
        }
    }

    switchMode(DirectIOConnectionSpreadType::DirectIOFailOver);
}


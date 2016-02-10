/*
 *	MetadataLoggingManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 02/02/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>
#include <chaos/common/metadata_logging/ErrorLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::message;
using namespace chaos::common::metadata_logging;

MetadataLoggingManager::MetadataLoggingManager() {
    message_channel = chaos::common::network::NetworkBroker::getInstance()->getRawMultiAddressMessageChannel(GlobalConfiguration::getInstance()->getMetadataServerAddressList());
    CHAOS_ASSERT(message_channel);
    
    //add default channels
    registerChannel("ErrorLoggingChannel",
                    METADATA_LOGGING_ERROR_LOGGING_CHANNEL_INSTANCER);
}

MetadataLoggingManager::~MetadataLoggingManager() {}

void MetadataLoggingManager::init(void *init_data) throw(chaos::CException) {
    
}

void MetadataLoggingManager::deinit() throw(chaos::CException) {
    boost::unique_lock<boost::mutex> wl(mutext_maps);
    for(MetadataLoggingInstancesMapIterator it = map_instance.begin();
        it != map_instance.end();
        it++) {
        if(it->second != NULL) {
            delete(it->second);
        }
    }
    map_instance.clear();
    map_instancer.clear();
}

MultiAddressMessageChannel *MetadataLoggingManager::getMessageChannelInstance() {
    return message_channel;
}

void MetadataLoggingManager::registerChannel(const std::string& channel_alias,
                                             chaos::common::utility::ObjectInstancer<AbstractMetadataLogChannel> *instancer) {
    map_instancer.insert(make_pair(channel_alias, instancer));
}

AbstractMetadataLogChannel *MetadataLoggingManager::getChannel(const std::string channel_alias) {
    boost::unique_lock<boost::mutex> wl(mutext_maps);
    //check if we have the channel
    if(map_instancer.count(channel_alias) == 0) return NULL;
    
    AbstractMetadataLogChannel *result = map_instancer[channel_alias]->getInstance();
    result->setMessageChannel(message_channel);
    map_instance.insert(make_pair(result->getInstanceUUID(), result));
    return result;
}

void MetadataLoggingManager::releaseChannel(AbstractMetadataLogChannel *channel_instance) {
    boost::unique_lock<boost::mutex> wl(mutext_maps);
    if(channel_instance == NULL) return;
    if(map_instance.count(channel_instance->getInstanceUUID()) == 0) return;
    //we can delete the instance
  
    map_instance.erase(channel_instance->getInstanceUUID());
    delete(channel_instance);
}
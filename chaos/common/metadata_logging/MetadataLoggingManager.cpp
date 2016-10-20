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
#include <chaos/common/metadata_logging/ErrorLoggingChannel.h>
#include <chaos/common/metadata_logging/StandardLoggingChannel.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>
#include <chaos/common/metadata_logging/BatchCommandLoggingChannel.h>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::pqueue;
using namespace chaos::common::network;
using namespace chaos::common::message;
using namespace chaos::common::metadata_logging;

#define MLM_INFO    INFO_LOG(MetadataLoggingManager)
#define MLM_DBG     DBG_LOG(MetadataLoggingManager)
#define MLM_ERR     ERR_LOG(MetadataLoggingManager)

MetadataLoggingManager::MetadataLoggingManager():
message_channel(NULL) {
    //add default channels
    registerChannel("StandardLoggingChannel",
                    METADATA_LOGGING_STANDARD_LOGGING_CHANNEL_INSTANCER);
    registerChannel("ErrorLoggingChannel",
                    METADATA_LOGGING_ERROR_LOGGING_CHANNEL_INSTANCER);
    registerChannel("BatchCommandLoggingChannel",
                    METADATA_LOGGING_COMMAND_LOGGING_CHANNEL_INSTANCER);
}

MetadataLoggingManager::~MetadataLoggingManager() {}

void MetadataLoggingManager::init(void *init_data) throw(chaos::CException) {
    if(GlobalConfiguration::getInstance()->getMetadataServerAddressList().size() > 0) {
        message_channel = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel(GlobalConfiguration::getInstance()->getMetadataServerAddressList());
        if(message_channel) {
            MLM_INFO << "We have got a message channel so we can forward the log";
        } else {
            MLM_ERR << "We have had error opening a message channel so all log will be stored locally[in future release]";
        }
    }

    CObjectProcessingPriorityQueue<CDataWrapper>::init(1);
}

void MetadataLoggingManager::deinit() throw(chaos::CException) {
    MLM_INFO << "Wait for queue will empty";
    CObjectProcessingPriorityQueue<CDataWrapper>::deinit(true);
    MLM_INFO << "Queue is empty";
    
    boost::unique_lock<boost::mutex> wl(mutext_maps);
    for(MetadataLoggingInstancesMapIterator it = map_instance.begin();
        it != map_instance.end();
        it++) {
        MLM_INFO << "Remove channel instance:"<< it->first;
        if(it->second != NULL) {
            delete(it->second);
        }
    }
    MLM_INFO << "All channel has been removed";
    map_instance.clear();
    map_instancer.clear();
    
    if(message_channel != NULL){NetworkBroker::getInstance()->disposeMessageChannel(message_channel); message_channel = NULL;}
}

void MetadataLoggingManager::registerChannel(const std::string& channel_alias,
                                             chaos::common::utility::ObjectInstancer<AbstractMetadataLogChannel> *instancer) {
    map_instancer.insert(make_pair(channel_alias, instancer));
}

AbstractMetadataLogChannel *MetadataLoggingManager::getChannel(const std::string channel_alias) {
    //chec if we are initilized
    if(getServiceState() != 1) return NULL;
    boost::unique_lock<boost::mutex> wl(mutext_maps);
    //check if we have the channel
    if(map_instancer.count(channel_alias) == 0) return NULL;
    
    AbstractMetadataLogChannel *result = map_instancer[channel_alias]->getInstance();
    result->setLoggingManager(this);
    map_instance.insert(make_pair(result->getInstanceUUID(), result));
    
    MLM_INFO << "Creted new channel instance " << result->getInstanceUUID() << " for " << channel_alias;
    return result;
}

void MetadataLoggingManager::releaseChannel(AbstractMetadataLogChannel *channel_instance) {
    //chec if we are initilized
    boost::unique_lock<boost::mutex> wl(mutext_maps);
    if(channel_instance == NULL) return;
    if(map_instance.count(channel_instance->getInstanceUUID()) == 0) return;
    //we can delete the instance
    
    map_instance.erase(channel_instance->getInstanceUUID());
    MLM_INFO << "Release channel instance " << channel_instance->getInstanceUUID();
    delete(channel_instance);
}

void MetadataLoggingManager::processBufferElement(CDataWrapper *log_entry,
                                                  ElementManagingPolicy& element_policy) throw(CException) {
    CHAOS_ASSERT(getServiceState() == 1);
    int err = 0;
    DEBUG_CODE(MLM_DBG << "forwarding log entry " << log_entry->getJSONString());
    
    //detach the entry from the queue
    element_policy.elementHasBeenDetached = true;
    
    if(message_channel) {
        if((err = sendLogEntry(log_entry))) {
            MLM_ERR << "Error forwarding log entry with code:" << err;
            //log entry need to be resubmitted or stored on disk (in future version)
            //delete(log_entry);
        } else {
            //log entry has been submitted
            MLM_DBG << "Successfully submited log entry";
        }
    } else {
        //no message channel available so we need to store the log
        MLM_DBG << "No channel for submitting log entry";
        delete(log_entry);
    }
}

int MetadataLoggingManager::sendLogEntry(chaos::common::data::CDataWrapper *log_entry) {
    CHAOS_ASSERT(getServiceState() == 1);

    int err = 0;
    //send message to mds and wait for ack
    std::auto_ptr<MultiAddressMessageRequestFuture> log_future = message_channel->sendRequestWithFuture(MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_RPC_DOMAIN,
                                                                                                        MetadataServerLoggingDefinitionKeyRPC::ACTION_NODE_LOGGING_SUBMIT_ENTRY,
                                                                                                        log_entry,
                                                                                                        2000);
    //wait for ack
    if(log_future->wait()) {
        //we have got semthing
        DEBUG_CODE(MLM_DBG << "Submition log entry has received ack with error\n " << log_future->getError() <<
                   "\n" << log_future->getErrorMessage() << "\n" <<
                   log_future->getErrorDomain(););
        if((err = log_future->getError())) {
            MLM_ERR << "Error forwarding log entry with code:" << err;
        } else {
            //log has been successfully forwarded
            DEBUG_CODE(MLM_DBG << "Log has been successfully forwarded");
        }
    } else {
        //we can't be able to send log to any mds server so detach it and need to store it
        //for later transmisison
        //log_future->detachMessageData();
        err = -10000;
    }
    return err;
}

int MetadataLoggingManager::pushLogEntry(chaos::common::data::CDataWrapper *log_entry,
                                         int32_t priority) {
    CHAOS_ASSERT(getServiceState() == 1);
    if(log_entry == NULL) return -10000;
    return CObjectProcessingPriorityQueue<CDataWrapper>::push(log_entry,
                                                              priority,
                                                              false);
}

/*
 *	EventDispatchManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 16/02/16 INFN, National Institute of Nuclear Physics
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

#include <ChaosMetadataServiceClient/event/EventDispatchManager.h>

#include <chaos/common/network/NetworkBroker.h>

#define EDM_LAPP INFO_LOG(EventDispatchManager)
#define EDM_LDBG DBG_LOG(EventDispatchManager)
#define EDM_LERR ERR_LOG(EventDispatchManager)

using namespace chaos::metadata_service_client::event;

//! default private constructor called only by ChaosMetadataServiceClient class
EventDispatchManager::EventDispatchManager(ClientSetting *_setting):
setting(_setting),
alert_event_channel(NULL) {
    
}

//! default destructor called only by ChaosMetadataServiceClient class
EventDispatchManager::~EventDispatchManager() {
    
}

void EventDispatchManager::init(void *init_data) throw (chaos::CException) {
    
    //allcoate the event channel
    alert_event_channel = NetworkBroker::getInstance()->getNewAlertEventChannel();
    if(alert_event_channel)
      alert_event_channel->activateChannelEventReception(this);
    
}

void EventDispatchManager::deinit() throw (chaos::CException) {
    if(alert_event_channel) {
        alert_event_channel->deactivateChannelEventReception(this);
        NetworkBroker::getInstance()->disposeEventChannel(alert_event_channel);
    }
}

void EventDispatchManager::registerEventHandler(AbstractEventHandler *handler) {
    if(handler == NULL) return;
    //write lock the map
    boost::unique_lock<boost::shared_mutex>(map_mutex);
    
    //generate the key
    HandlerMapKeys key(handler->getType(), handler->getSubType());
    
    //insert handler
    if(map_handler.count(key) == 0) {
        map_handler.insert(make_pair(key, HandlerMapValue()));
    }
    map_handler[key].insert(handler);
}

void EventDispatchManager::deregisterEventHandler(AbstractEventHandler *handler) {
    if(handler != NULL) return;
    
    //write lock the map
    boost::unique_lock<boost::shared_mutex>(map_mutex);
    
    //generate the key
    HandlerMapKeys key(handler->getType(), handler->getSubType());
    
    //erase handler
    map_handler[key].erase(handler);
    if(map_handler[key].size() == 0) {
        //remove the key
        map_handler.erase(key);
    }
}

void EventDispatchManager::handleEvent(const common::event::EventDescriptor * const event) {
    //read lock the map
    boost::shared_lock<boost::shared_mutex>(map_mutex);
    DEBUG_CODE(EDM_LDBG << "Event received type:"<< event->getEventType() << " with code:" << event->getSubCode(););
    
    //generate the key
    HandlerMapKeys key(event->getEventType(), event->getSubCode());
    
    for(HandlerMapValueIterator it = map_handler[key].begin();
        it != map_handler[key].end();
        it++) {
        (*it)->handleEvent(event);
    }
    
}

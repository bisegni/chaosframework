/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos_metadata_service_client/event/EventDispatchManager.h>

#include <chaos/common/network/NetworkBroker.h>

#define EDM_LAPP INFO_LOG(EventDispatchManager)
#define EDM_LDBG DBG_LOG(EventDispatchManager)
#define EDM_LERR ERR_LOG(EventDispatchManager)

using namespace chaos::metadata_service_client::event;
using namespace chaos::common::network;
//! default private constructor called only by chaos_metadata_service_client class
EventDispatchManager::EventDispatchManager(chaos::metadata_service_client::ClientSetting *_setting):
setting(_setting),
alert_event_channel(NULL) {
    
}

//! default destructor called only by chaos_metadata_service_client class
EventDispatchManager::~EventDispatchManager() {
    
}

void EventDispatchManager::init(void *init_data)  {
    
    //allcoate the event channel
    alert_event_channel = NetworkBroker::getInstance()->getNewAlertEventChannel();
    if(alert_event_channel)
      alert_event_channel->activateChannelEventReception(this);
    
}

void EventDispatchManager::deinit()  {
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
    if(handler == NULL) return;
    
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

void EventDispatchManager::handleEvent(const chaos::common::event::EventDescriptor * const event) {
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

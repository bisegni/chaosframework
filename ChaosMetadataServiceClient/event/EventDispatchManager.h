/*
 *	EventDispatchManager.h
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

#ifndef __CHAOSFramework__EventDispatchManager_h
#define __CHAOSFramework__EventDispatchManager_h


#include <ChaosMetadataServiceClient/event/AbstractEventHandler.h>
#include <ChaosMetadataServiceClient/metadata_service_client_types.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/event/event.h>
#include <chaos/common/utility/InizializableService.h>

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/thread.hpp>

#define GET_CHAOS_API_PTR(api_name)\
chaos::metadata_service_client::ChaosMetadataServiceClient::getInstance()->getApiProxy<api_name>()

namespace chaos {
    namespace metadata_service_client {
        //! forward declaration
        class ChaosMetadataServiceClient;
        
        namespace event {
            
            typedef std::pair<unsigned int, unsigned int>       HandlerMapKeys;
            typedef std::set<AbstractEventHandler*>             HandlerMapValue;
            typedef std::set<AbstractEventHandler*>::iterator   HandlerMapValueIterator;
            
            CHAOS_DEFINE_MAP_FOR_TYPE(HandlerMapKeys, HandlerMapValue, HandlerMap);
            
            //! Manager for the creation of endler for a determinated kind of chaos event
            /*!
             It help the usage of some chaos event usefull to the mds environment
             */
            class EventDispatchManager:
            protected chaos::EventAction,
            public chaos::common::utility::InizializableService {
                friend class chaos::common::utility::InizializableServiceContainer<EventDispatchManager>;
                friend class chaos::metadata_service_client::ChaosMetadataServiceClient;
                
                ClientSetting *setting;
                
                boost::shared_mutex     map_mutex;
                HandlerMap              map_handler;
                
                //alert event channel
                common::event::channel::AlertEventChannel *alert_event_channel;
                
            protected:
                EventDispatchManager(ClientSetting *_setting);
                
                ~EventDispatchManager();
                
                //!inherited by InizializableService
                void init(void *init_data) throw (chaos::CException);
                
                //!inherited by InizializableService
                void deinit() throw (chaos::CException);
                
                void handleEvent(const chaos::common::event::EventDescriptor * const event);
            public:
                void registerEventHandler(AbstractEventHandler *handler);
                void deregisterEventHandler(AbstractEventHandler *handler);
            };
        }
    }
}

#endif /* __CHAOSFramework__EventDispatchManager_h */

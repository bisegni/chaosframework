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

#ifndef __CHAOSFramework__EventDispatchManager_h
#define __CHAOSFramework__EventDispatchManager_h


#include <chaos_metadata_service_client/event/AbstractEventHandler.h>
#include <chaos_metadata_service_client/metadata_service_client_types.h>

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

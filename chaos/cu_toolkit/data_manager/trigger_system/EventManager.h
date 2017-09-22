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

#ifndef __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h
#define __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h

#include <chaos/cu_toolkit/data_manager/trigger_system/trigger_system.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {

                
                //!this is the main class where all other sublayer fire event with data
                /*!
                 Event dispatcher collect even fired by other sublayer and attach it to algorithm execution
                 */
                class EventManager {
                    boost::shared_mutex                         mutex_event_container;
                public:
                    EventManager();
                    ~EventManager();

                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework_C1346138_0E86_4813_9DE2_2C2B525C6C1C_EventManager_h */

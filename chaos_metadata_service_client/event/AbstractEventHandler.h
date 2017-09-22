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

#ifndef __CHAOSFramework__AbstractEventHandler_h
#define __CHAOSFramework__AbstractEventHandler_h

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service_client {
        
        namespace event {
            //! forward decalration
            class EventDispatchManager;
            
            class AbstractEventHandler {
                friend class EventDispatchManager;
                const chaos::common::event::EventType type;
                const int subtype;
            protected:
                virtual void handleEvent(const chaos::common::event::EventDescriptor * const event) = 0;
            public:
                AbstractEventHandler(chaos::common::event::EventType _type,
                                     const unsigned int _subtype);
                virtual ~AbstractEventHandler();
                const chaos::common::event::EventType& getType();
                const unsigned int getSubType();
            };
        }
    }
}


#endif /* __CHAOSFramework__AbstractEventHandler_h */

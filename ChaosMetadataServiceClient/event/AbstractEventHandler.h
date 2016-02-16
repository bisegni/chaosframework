/*
 *	AbstractEventHandler.h
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

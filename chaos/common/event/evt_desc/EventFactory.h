/*
 *	EventFactory.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 22/08/12.
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

#ifndef __CHAOSFramework__EventFactory__
#define __CHAOSFramework__EventFactory__

#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/exception/CException.h>

namespace chaos {
    namespace event{
        class EventFactory {
        public:
            static EventDescriptor *getEventInstance(const unsigned char * serializedEvent, uint16_t length) throw (CException);
        };

    }
}

#endif /* defined(__CHAOSFramework__EventFactory__) */

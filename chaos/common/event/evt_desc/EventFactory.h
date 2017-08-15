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

#ifndef __CHAOSFramework__EventFactory__
#define __CHAOSFramework__EventFactory__

#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/exception/exception.h>

namespace chaos {
    namespace common {
        namespace event{
            class EventFactory {
            public:
                static EventDescriptor *getEventInstance(const unsigned char * serializedEvent, uint16_t length) throw (CException);
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__EventFactory__) */

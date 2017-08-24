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

#ifndef __CHAOSFramework_E85E5697_253B_41EE_BB6B_274C198201CB_AbstractListenerContainer_h
#define __CHAOSFramework_E85E5697_253B_41EE_BB6B_274C198201CB_AbstractListenerContainer_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/utility/AbstractListener.h>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace utility {
            
            CHAOS_DEFINE_SET_FOR_TYPE(AbstractListener*, SetListner);
            CHAOS_DEFINE_LOCKABLE_OBJECT(SetListner, SetListnerLO);
            
            //!abstract listener container
            class AbstractListenerContainer {
                SetListnerLO listeners;
            protected:
                virtual void fireToListener(unsigned int fire_code,
                                            AbstractListener *listener_to_fire) = 0;
                void fire(unsigned int fire_code);
            public:
                AbstractListenerContainer();
                virtual ~AbstractListenerContainer();
                
                void addListener(AbstractListener *new_listener);
                void removeListener(AbstractListener *erase_listener);
            };
        }
    }
}

#endif /* __CHAOSFramework_E85E5697_253B_41EE_BB6B_274C198201CB_AbstractListenerContainer_h */

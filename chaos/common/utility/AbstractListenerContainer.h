/*
 *	AbstractListenerContainer.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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
            
            //!abstract listener container
            class AbstractListenerContainer {
                chaos::common::utility::LockableObject<SetListner> listeners;

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

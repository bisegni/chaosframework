/*
 *	AbstractListenerContainer.cpp
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

#include <chaos/common/utility/AbstractListenerContainer.h>

using namespace chaos::common::utility;

AbstractListenerContainer::AbstractListenerContainer() {}

AbstractListenerContainer::~AbstractListenerContainer() {
    listeners.clear();
}

void AbstractListenerContainer::addListener(AbstractListener *new_listener) {
    boost::unique_lock<boost::shared_mutex> wl(mutex_listener);
    listeners.insert(new_listener);
}

void AbstractListenerContainer::removeListener(AbstractListener *erase_listener) {
    boost::unique_lock<boost::shared_mutex> wl(mutex_listener);
    listeners.erase(erase_listener);
}

void AbstractListenerContainer::fire(unsigned int fire_code) {
    boost::shared_lock<boost::shared_mutex> rl(mutex_listener);
    for(SetListnerIterator it = listeners.begin(),
        end = listeners.end();
        it != end;
        it++) {
        fireToListener(fire_code,
                       *it);
    }
}

void AbstractListenerContainer::fireToListener(unsigned int fire_code,
                                               AbstractListener *listener_to_fire) {
    
}

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

#include <chaos/common/utility/AbstractListenerContainer.h>

using namespace chaos::common::utility;

AbstractListenerContainer::AbstractListenerContainer() {}

AbstractListenerContainer::~AbstractListenerContainer() {listeners().clear();}

void AbstractListenerContainer::addListener(AbstractListener *new_listener) {
    SetListnerLOWriteLock wl = listeners.getWriteLockObject();
    listeners().insert(new_listener);
}

void AbstractListenerContainer::removeListener(AbstractListener *erase_listener) {
    SetListnerLOWriteLock rl = listeners.getWriteLockObject();

    listeners().erase(erase_listener);
}

void AbstractListenerContainer::fire(unsigned int fire_code) {
    SetListnerLOReadLock rl = listeners.getReadLockObject();
    for(SetListnerIterator it = listeners().begin(),
        end = listeners().end();
        it != end;
        it++) {
        fireToListener(fire_code,
                       *it);
    }
}

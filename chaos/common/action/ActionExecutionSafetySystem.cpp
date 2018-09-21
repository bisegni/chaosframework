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

#include <chaos/common/action/ActionExecutionSafetySystem.h>

using namespace chaos;

ActionExecutionSafetySystem::ActionExecutionSafetySystem():
fired(false),
enabled(false){}
ActionExecutionSafetySystem::~ActionExecutionSafetySystem() {}

bool ActionExecutionSafetySystem::isFired(){
    AESS_ReadLock lock(aessMutext);
    return fired;
}

bool ActionExecutionSafetySystem::setFiredWriteLocked(bool _fired){
    AESS_WriteLock lock(aessMutext);
        //return true only if  fired and enabled are true
    return setFired(_fired);
}

bool ActionExecutionSafetySystem::setFired(bool _fired){
    fired =_fired && enabled;
    return fired;
}

bool ActionExecutionSafetySystem::isEnabled(){
    AESS_ReadLock lock(aessMutext);
    return enabled;
}

bool ActionExecutionSafetySystem::setEnabled(bool _enabled){
    AESS_WriteLock lock(aessMutext);
    if(_enabled && fired) {
            //attent the termination
    }
    return enabled =_enabled;
}

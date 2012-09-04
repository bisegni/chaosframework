/*
 *	ActionExecutionSafetySystem.cpp
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 02/09/12.
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

#include <chaos/common/action/ActionExecutionSafetySystem.h>

using namespace chaos;

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

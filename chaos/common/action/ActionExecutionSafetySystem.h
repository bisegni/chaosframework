/*
 *	ActionExecutionSafetySystem.h
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

#ifndef __CHAOSFramework__ActionExecutionSafetySystem__
#define __CHAOSFramework__ActionExecutionSafetySystem__

#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <string>
#include <boost/thread.hpp>
#include <boost/thread/locks.hpp>

namespace chaos {
    
    using namespace std;
    using namespace boost;
    
    
    typedef boost::shared_mutex                         AESS_SharedLock;
    typedef boost::unique_lock< boost::shared_mutex >   AESS_WriteLock;
    typedef boost::shared_lock< boost::shared_mutex >   AESS_ReadLock;
 
    class EventTypeScheduler;
    
    class ActionExecutionSafetySystem {
        friend class EventTypeScheduler;
        
        bool fired;
        bool enabled;
        
    protected:
        AESS_SharedLock aessMutext;
        
        bool setFiredWriteLocked(bool _fired);
        bool setFired(bool _fired);
        bool setEnabled(bool);
        
    public:
        bool isFired();
        bool isEnabled();
    };
}

#endif /* defined(__CHAOSFramework__ActionExecutionSafetySystem__) */

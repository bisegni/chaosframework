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
        ActionExecutionSafetySystem();
        ~ActionExecutionSafetySystem();
        bool isFired();
        bool isEnabled();
    };
}

#endif /* defined(__CHAOSFramework__ActionExecutionSafetySystem__) */

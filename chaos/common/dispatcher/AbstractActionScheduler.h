/*
 *	AbstractActionScheduler.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 05/01/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__F9A52C6_3A53_46C7_B6C8_138000FCD4D7_AbstractActionScheduler_h
#define __CHAOSFramework__F9A52C6_3A53_46C7_B6C8_138000FCD4D7_AbstractActionScheduler_h

#include <chaos/common/action/DomainActions.h>
#include <chaos/common/dispatcher/AbstractCommandDispatcher.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos {
    
    class AbstractActionScheduler {
        friend class CommandDispatcher;

    protected:
        //!reference to global dispatcher used to resubmit sub command
        AbstractCommandDispatcher *dispatcher;
    public:
        AbstractActionScheduler();
        ~AbstractActionScheduler();
        /*
         
         */
        void setDispatcher(AbstractCommandDispatcher *newDispatcher);

    };
    
}

#endif /* __CHAOSFramework__F9A52C6_3A53_46C7_B6C8_138000FCD4D7_AbstractActionScheduler_h */

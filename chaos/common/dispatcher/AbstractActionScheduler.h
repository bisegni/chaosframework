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

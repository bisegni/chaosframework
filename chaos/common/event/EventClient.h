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

#ifndef __CHAOSFramework__EventClient__
#define __CHAOSFramework__EventClient__
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/event/EventForwarder.h>
#include <chaos/common/pqueue/CObjectProcessingPriorityQueue.h>
#include <chaos/common/utility/StartableService.h>
namespace chaos {
    namespace common {
        namespace network {
            class NetworkBroker;
        }
        
        namespace event {
            
            //!Event client base class
            /*!
             
             */
            class EventClient:
            public common::utility::NamedService,
            public EventForwarder,
            public chaos::common::utility::StartableService {
                friend class chaos::common::network::NetworkBroker;
                
            protected:
                uint8_t threadNumber;
                //Default Server Constructor
                EventClient(std::string alias);
                
                /*
                 init the event adapter
                 */
                virtual void init(void*);
                
                /*
                 start the event adapter
                 */
                virtual void start();
                
                //-----------------------
                virtual void stop(){};
                
                /*
                 deinit the event adapter
                 */
                virtual void deinit();
                
            public:
                void setThreadNumber(unsigned int newThreadNumber);
                unsigned int getThreadNumber();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__EventClient__) */

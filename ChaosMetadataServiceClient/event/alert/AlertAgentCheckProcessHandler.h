/*
 *	AlertAgentCheckProcessHandler.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 16/02/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__CD6CD4D_61F0_4997_83B6_352E72DEFD00_AlertAgentCheckProcessHandler_h
#define __CHAOSFramework__CD6CD4D_61F0_4997_83B6_352E72DEFD00_AlertAgentCheckProcessHandler_h

#include <ChaosMetadataServiceClient/event/alert/AlertEventHandler.h>

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service_client {
        namespace event {
            namespace alert {
                
                class AlertAgentCheckProcessHandler:
                public AlertEventHandler {
                protected:
                    void handleEvent(const chaos::common::event::EventDescriptor * const event);
                public:
                    AlertAgentCheckProcessHandler();
                    ~AlertAgentCheckProcessHandler();
                    virtual void handleAgentEvent(const std::string& agent_uid,
                                                  const int32_t& check_result) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__CD6CD4D_61F0_4997_83B6_352E72DEFD00_AlertAgentCheckProcessHandler_h */

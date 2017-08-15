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

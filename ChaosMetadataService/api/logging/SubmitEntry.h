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

#ifndef __CHAOSFramework__SubmitEntry_h
#define __CHAOSFramework__SubmitEntry_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace logging {
                
                //! Add A log entry
                /*!
                 
                 */
                class SubmitEntry:
                public AbstractApi {
                    chaos::common::event::channel::AlertEventChannel *alert_event_channel;
                    //!complete the log entry for the error logging domain
                    void completeLogEntry(chaos::common::data::CDataWrapper *api_data,
                                          persistence::data_access::LogEntry& new_log_entry);
                public:
                    SubmitEntry();
                    ~SubmitEntry();
                    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__SubmitEntry_h */

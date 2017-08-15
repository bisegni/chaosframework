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

#ifndef __CHAOSFramework__GetLogForSourceUID_h
#define __CHAOSFramework__GetLogForSourceUID_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                struct LogEntry;
            }
        }
        namespace api {
            namespace logging {
                
                //! Return logging for a device uid
                /*!
                 The logging are paged so in query can be gave the last sequence id
                 */
                class GetLogForSourceUID:
                public AbstractApi {
                    inline ChaosUniquePtr<chaos::common::data::CDataWrapper> converEntry(persistence::data_access::LogEntry& log_entry);
                public:
                    GetLogForSourceUID();
                    ~GetLogForSourceUID();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__GetLogForSourceUID_h */

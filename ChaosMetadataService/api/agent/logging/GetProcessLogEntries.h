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

#ifndef __CHAOSFramework__DD5D79C_FFEA_4F9D_9C9C_13C12DC5E5A0_GetProcessLogEntries_h
#define __CHAOSFramework__DD5D79C_FFEA_4F9D_9C9C_13C12DC5E5A0_GetProcessLogEntries_h

#include "../../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                namespace logging {
                    //! Get a determinated number of process log entries
                    /*!
                     
                     */
                    CHAOS_MDS_DEFINE_API_CLASS(GetProcessLogEntries);
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__DD5D79C_FFEA_4F9D_9C9C_13C12DC5E5A0_GetProcessLogEntries_h */

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

#ifndef __CHAOSFramework__47A5D36_3CE2_4FEA_AF2E_5518334555A6_AgentLoggingApiGroup_h
#define __CHAOSFramework__47A5D36_3CE2_4FEA_AF2E_5518334555A6_AgentLoggingApiGroup_h

#include "../../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                namespace logging {
                    //! api group for the managment of the logging services
                    DECLARE_CLASS_FACTORY(AgentLoggingApiGroup, AbstractApiGroup) {
                        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(AgentLoggingApiGroup)
                    public:
                        AgentLoggingApiGroup();
                        ~AgentLoggingApiGroup();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__47A5D36_3CE2_4FEA_AF2E_5518334555A6_AgentLoggingApiGroup_h */

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

#ifndef __CHAOSFramework_F4144453_0879_4A8F_BAD0_2854B9C14D1F_LoadAgentDescription_h
#define __CHAOSFramework_F4144453_0879_4A8F_BAD0_2854B9C14D1F_LoadAgentDescription_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                
                //! List all unit server configured with agent
                /*!
                 
                 */
                class LoadAgentDescription:
                public AbstractApi {
                public:
                    LoadAgentDescription();
                    ~LoadAgentDescription();
                    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data);
                };
            }
        }
    }
}
#endif /* __CHAOSFramework_F4144453_0879_4A8F_BAD0_2854B9C14D1F_LoadAgentDescription_h */

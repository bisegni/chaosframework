/*
 * Copyright 2012, 24/01/2018 INFN
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

#ifndef __CHAOSFramework_EFEASD10_E100_4081_8C2E_8DASR3C5C5E8_NodeOperation_h
#define __CHAOSFramework_EFEASD10_E100_4081_8C2E_8DASR3C5C5E8_NodeOperation_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                
                //! Return an agent associated to node
                /*!
                 */
                CHAOS_MDS_DEFINE_API_CLASS(GetAgentForNode);
            }
        }
    }
}
#endif /* __CHAOSFramework_EFEASD10_E100_4081_8C2E_8DASR3C5C5E8_NodeOperation_h s*/


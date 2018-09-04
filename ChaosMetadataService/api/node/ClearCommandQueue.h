/*
 * Copyright 2012, 05/02/2018 INFN
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

#ifndef __CHAOSFramework__ClearCommandQueue__
#define __CHAOSFramework__ClearCommandQueue__

#include "ForwardNodeRpcMessage.h"
namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace node {
                //! Call feature api for the sandbox
                CHAOS_MDS_DEFINE_API_CLASS(ClearCommandQueue);
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__ClearCommandQueue__) */

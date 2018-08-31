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

#ifndef __CHAOSFramework_DB7DBA67_9E18_4B16_A6C5_75E4373A2587_UploadChunk_h
#define __CHAOSFramework_DB7DBA67_9E18_4B16_A6C5_75E4373A2587_UploadChunk_h

#include "../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace deploy {
                
                //! remove an association from an angen and a node
                /*!
                 
                 */
                class UploadChunk:
                public AbstractApi {
                public:
                    UploadChunk();
                    ~UploadChunk();
                    chaos::common::data::CDWUniquePtr execute(chaos::common::data::CDWUniquePtr api_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_DB7DBA67_9E18_4B16_A6C5_75E4373A2587_UploadChunk_h */

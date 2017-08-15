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

#ifndef __CHAOSFramework__3F277F9_C5C0_4313_AAFA_4454971B6E53_DeployApiGroup_h
#define __CHAOSFramework__3F277F9_C5C0_4313_AAFA_4454971B6E53_DeployApiGroup_h

#include "../AbstractApiGroup.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace deploy {
                
                //! api group for the managment of the logging services
                DECLARE_CLASS_FACTORY(DeployApiGroup, AbstractApiGroup) {
                    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(DeployApiGroup)
                public:
                    DeployApiGroup();
                    ~DeployApiGroup();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__3F277F9_C5C0_4313_AAFA_4454971B6E53_DeployApiGroup_h */

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

#ifndef __CHAOSFramework__F94F822_58ED_432B_8D65_FCF227A0B77A_SaveScript_h
#define __CHAOSFramework__F94F822_58ED_432B_8D65_FCF227A0B77A_SaveScript_h

#include "../AbstractApi.h"

#include <chaos_service_common/data/script/Script.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! Create a new script element in persistent database
                /*!
                 Perform the creation of new script with all data passed in input
                 */
                CHAOS_MDS_DEFINE_API(SaveScript)
            }
        }
    }
}

#endif /* __CHAOSFramework__F94F822_58ED_432B_8D65_FCF227A0B77A_SaveScript_h */

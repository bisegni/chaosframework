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

#ifndef __CHAOSFramework_FECD0277_DFF7_4945_8FFF_5FCBECB839D2_AbstractListener_h
#define __CHAOSFramework_FECD0277_DFF7_4945_8FFF_5FCBECB839D2_AbstractListener_h

#include <string>

namespace chaos {
    namespace common {
        namespace utility {
            class AbstractListener {
                const std::string listener_uuid;

            public:
                AbstractListener();
                virtual ~AbstractListener();
            };
        }
    }
}

#endif /* __CHAOSFramework_FECD0277_DFF7_4945_8FFF_5FCBECB839D2_AbstractListener_h */

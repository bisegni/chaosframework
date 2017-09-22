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

#ifndef __CHAOSFramework__CEF0B33_9C71_421E_8464_9A78CD98356E_SearchScript_h
#define __CHAOSFramework__CEF0B33_9C71_421E_8464_9A78CD98356E_SearchScript_h

#include "../AbstractApi.h"

#include <chaos_service_common/data/script/Script.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace script {
                //! execute a search on all script
                /*!
                 Search and return the scrip found according to search string
                 */
                class SearchScript:
                public AbstractApi {
                public:
                    SearchScript();
                    ~SearchScript();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__CEF0B33_9C71_421E_8464_9A78CD98356E_SearchScript_h */

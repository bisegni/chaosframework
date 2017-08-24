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

#ifndef __CHAOSFramework_A3F1DA1B_A9A1_48A3_854D_E2898C296342_SetSnapshotDatasetsForNode_h
#define __CHAOSFramework_A3F1DA1B_A9A1_48A3_854D_E2898C296342_SetSnapshotDatasetsForNode_h

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace service {
                
                //! Get the list of all node in snapshot
                /*!
                 
                 */
                class SetSnapshotDatasetsForNode:
                public AbstractApi {
                public:
                    SetSnapshotDatasetsForNode();
                    ~SetSnapshotDatasetsForNode();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_A3F1DA1B_A9A1_48A3_854D_E2898C296342_SetSnapshotDatasetsForNode_h */

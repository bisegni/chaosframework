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

#ifndef __CHAOSFramework__GetCurrentDataset__
#define __CHAOSFramework__GetCurrentDataset__

#include "../AbstractApi.h"

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace control_unit {
                class GetCurrentDataset:
                public AbstractApi {
                    ChaosSharedPtr<chaos::common::data::CDataWrapper> mergeDatasetAttributeWithSetup(ChaosSharedPtr<chaos::common::data::CDataWrapper> element_in_dataset,
                                                                                   ChaosSharedPtr<chaos::common::data::CDataWrapper> element_in_setup);
                public:
                    GetCurrentDataset();
                    ~GetCurrentDataset();
                    chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                               bool& detach_data) throw(chaos::CException);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__GetCurrentDataset__) */

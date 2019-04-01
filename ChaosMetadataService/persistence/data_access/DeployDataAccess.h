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

#ifndef __CHAOSFramework__027F1ED_9006_434C_AFBD_428F426F199F_DeployDataAccess_h
#define __CHAOSFramework__027F1ED_9006_434C_AFBD_428F426F199F_DeployDataAccess_h

#include "../persistence.h"

#include <chaos/common/chaos_types.h>

#include <chaos_service_common/data/data.h>
#include <chaos_service_common/persistence/data_access/AbstractDataAccess.h>
namespace chaos {
    namespace metadata_service {
        namespace persistence {
            namespace data_access {
                //!
                class DeployDataAccess:
                public chaos::service_common::persistence::data_access::AbstractDataAccess {
                    
                public:
                    DECLARE_DA_NAME
                    
                    //! default constructor
                    DeployDataAccess();
                    
                    //!default destructor
                    ~DeployDataAccess();
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__027F1ED_9006_434C_AFBD_428F426F199F_DeployDataAccess_h */

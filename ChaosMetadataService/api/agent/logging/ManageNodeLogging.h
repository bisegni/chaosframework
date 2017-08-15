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

#ifndef __CHAOSFramework__5E19371_6581_4BB8_9991_F8B8962CC80C_ManageNodeLogging_h
#define __CHAOSFramework__5E19371_6581_4BB8_9991_F8B8962CC80C_ManageNodeLogging_h

#include "../../AbstractApi.h"

#include <chaos/common/event/event.h>

namespace chaos {
    namespace metadata_service {
        namespace api {
            namespace agent {
                namespace logging {
                    //! remove an association from an angen and a node
                    /*!
                     
                     */
                    class ManageNodeLogging:
                    public AbstractApi {
                        
                    public:
                        ManageNodeLogging();
                        ~ManageNodeLogging();
                        chaos::common::data::CDataWrapper *execute(chaos::common::data::CDataWrapper *api_data,
                                                                   bool& detach_data);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework__5E19371_6581_4BB8_9991_F8B8962CC80C_ManageNodeLogging_h */

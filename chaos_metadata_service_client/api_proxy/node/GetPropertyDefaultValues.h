/*
 * Copyright 2012, 06/09/2017 INFN
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

#ifndef __CHAOSFramework__951CA3E_DFD6_4AD8_9901_4A161563AC9B_GetPropertyDefaultValues_h
#define __CHAOSFramework__951CA3E_DFD6_4AD8_9901_4A161563AC9B_GetPropertyDefaultValues_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>
#include <chaos/common/property/property.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! submit an update to a property for a node
                class GetPropertyDefaultValues:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(GetPropertyDefaultValues)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(GetPropertyDefaultValues)
                public:
                    /*!
                     \param node_unique_id the unique id of node for that hown the property defaults
                     */
                    ApiProxyResult execute(const std::string& node_unique_id);
                    
                    static void deserialize(chaos::common::data::CDataWrapper& serialization,
                                            chaos::common::property::PropertyGroupVector& property_group_vector);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__951CA3E_DFD6_4AD8_9901_4A161563AC9B_GetPropertyDefaultValues_h */

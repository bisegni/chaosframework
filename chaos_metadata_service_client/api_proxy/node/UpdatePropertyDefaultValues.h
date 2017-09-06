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

#ifndef __CHAOSFramework_C9796C05_B825_4312_ADC6_DDBA3F7ADCC7_UpdatePropertyDefaultValues_h
#define __CHAOSFramework_C9796C05_B825_4312_ADC6_DDBA3F7ADCC7_UpdatePropertyDefaultValues_h

#include <chaos_metadata_service_client/api_proxy/ApiProxy.h>
#include <chaos/common/property/property.h>

namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace node {
                
                //! submit an update to a property for a node
                class UpdatePropertyDefaultValues:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(UpdatePropertyDefaultValues)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(UpdatePropertyDefaultValues)
                public:
                    /*!
                     \param node_unique_id the unique id of a command
                     \param node_property_groups_list the property group wit property valu
                     */
                    ApiProxyResult execute(const std::string& node_unique_id,
                                           chaos::common::property::PropertyGroup& node_property_group);
                    
                    /*!
                     \param node_unique_id the unique id of a command
                     */
                    ApiProxyResult execute(const std::string& node_unique_id,
                                           chaos::common::property::PropertyGroupVector& node_property_group_vector);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework_C9796C05_B825_4312_ADC6_DDBA3F7ADCC7_UpdatePropertyDefaultValues_h */

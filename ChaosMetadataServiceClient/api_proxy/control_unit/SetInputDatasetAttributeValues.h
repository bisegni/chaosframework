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

#ifndef __CHAOSFramework__SetInputDatasetAttributeValues__
#define __CHAOSFramework__SetInputDatasetAttributeValues__

#include <ChaosMetadataServiceClient/api_proxy/ApiProxy.h>

#include <boost/shared_ptr.hpp>
namespace chaos {
    namespace metadata_service_client {
        namespace api_proxy {
            namespace control_unit {
                
                
                struct InputDatasetAttributeChangeValue {
                    std::string attribute_name;
                    std::string value;
                    
                    InputDatasetAttributeChangeValue(const std::string& _attribute_name,
                                                     const std::string& _value);
                    
                };
                
                struct ControlUnitInputDatasetChangeSet {
                    std::string cu_uid;
                    std::vector< ChaosSharedPtr<InputDatasetAttributeChangeValue> > change_set;
                    ControlUnitInputDatasetChangeSet(const std::string _cu_uid,
                                                     const std::vector< ChaosSharedPtr<InputDatasetAttributeChangeValue> >& _change_set);
                };
                
                //! return the current control unit dataset
                class SetInputDatasetAttributeValues:
                public chaos::metadata_service_client::api_proxy::ApiProxy {
                    API_PROXY_CLASS(SetInputDatasetAttributeValues)
                protected:
                    //! default constructor
                    API_PROXY_CD_DECLARATION(SetInputDatasetAttributeValues)
                public:
                    //! perform the start and stop operation for a control unit
                    /*!
                     \param cu_unique_id control unit id for wich we need the dataset
                     */
                    ApiProxyResult execute(const std::vector< ChaosSharedPtr<ControlUnitInputDatasetChangeSet> >& change_set);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SetInputDatasetAttributeValues__) */

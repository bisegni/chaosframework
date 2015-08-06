/*
 *	SetInputDatasetAttributeValues.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
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
                    std::vector< boost::shared_ptr<InputDatasetAttributeChangeValue> > change_set;
                    ControlUnitInputDatasetChangeSet(const std::string _cu_uid,
                                                     const std::vector< boost::shared_ptr<InputDatasetAttributeChangeValue> >& _change_set);
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
                    ApiProxyResult execute(const std::vector< boost::shared_ptr<ControlUnitInputDatasetChangeSet> >& change_set);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__SetInputDatasetAttributeValues__) */

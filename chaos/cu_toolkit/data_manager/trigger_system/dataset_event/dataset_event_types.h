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

#ifndef __CHAOSFramework_EBE81686_95F6_480A_BC26_6F5F0609F348_dataset_event_types_h
#define __CHAOSFramework_EBE81686_95F6_480A_BC26_6F5F0609F348_dataset_event_types_h

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    
                    //! define the types for the event target dataset attribute
                    typedef enum ETDatasetAttributeType {
                        ETDatasetAttributeTypePreChange,
                        ETDatasetAttributeTypePostChange
                    }ETDatasetAttributeType;
                    
                    //! define the result of a attribute change event/consumer operation
                    typedef enum ETDatasetAttributeResult {
                        //!value has been accepted
                        ETDatasetAttributeResultOK,
                        ETDatasetAttributeResultRejected
                    }ETDatasetAttributeResult;
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_EBE81686_95F6_480A_BC26_6F5F0609F348_dataset_event_types_h */

/*
 *	dataset_event_types.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EBE81686_95F6_480A_BC26_6F5F0609F348_dataset_event_types_h
#define __CHAOSFramework_EBE81686_95F6_480A_BC26_6F5F0609F348_dataset_event_types_h

#include <chaos/cu_toolkit//data_manager/trigger_system/trigger_system_types.h>

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

/*
 *	RegisterEnviroment.h
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

#ifndef __CHAOSFramework_EC1AE67E_A602_4C78_9F15_67A55883D782_RegisterEnviroment_h
#define __CHAOSFramework_EC1AE67E_A602_4C78_9F15_67A55883D782_RegisterEnviroment_h

#include <chaos/cu_toolkit/data_manager/trigger_system/RegisterEventConsumerEnvironment.h>

#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetEvent.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetConsumer.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetSubject.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    
                    //!define the register environmento for dataset attribute trigger
                    class DatasetRegisterEvenironment:
                    public RegisterEventConsumerEnvironment<DatasetEvent,
                    DatasetConsumer,
                    DatasetSubject,
                    ETDatasetAttributeType> {
                    public:
                        DatasetRegisterEvenironment();
                        ~DatasetRegisterEvenironment();
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_EC1AE67E_A602_4C78_9F15_67A55883D782_RegisterEnviroment_h */

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

#ifndef __CHAOSFramework_EC1AE67E_A602_4C78_9F15_67A55883D782_RegisterEnviroment_h
#define __CHAOSFramework_EC1AE67E_A602_4C78_9F15_67A55883D782_RegisterEnviroment_h

#include <chaos/common/trigger/TriggerExecutionEnviroment.h>

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
                    public chaos::common::trigger::TriggerExecutionEnviroment<DatasetSubject, ETDatasetAttributeType> {
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

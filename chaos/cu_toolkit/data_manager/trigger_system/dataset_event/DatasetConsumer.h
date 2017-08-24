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

#ifndef __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h
#define __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h

#include <chaos/common/trigger/AbstractConsumer.h>
#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetSubject.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/dataset_event_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    namespace chaos_trigger = chaos::common::trigger;
                    //!define the abstract consumer for AbstractEventSetDSAttribute
                    
                    //------create the consumer
//                    CHAOS_TRIGGER_CONSUMER_OPEN_DESCRIPTION(SubjectConsumerIncrement,
//                                                            SubjectConsumer)
//                    CHAOS_TRIGGER_CONSUMER_ADD_PROPERTY("offset", "good property", chaos::DataType::TYPE_INT32);
//                    CHAOS_TRIGGER_CONSUMER_ADD_DEFINITION(SubjectConsumerIncrement,
//                                                          SubjectConsumer,
//                                                          "Preform integer increment")
//                    //we need to declare the custom consumer event
//                    ConsumerResult consumeEvent(TriggerDataEventType event_type,
//                                                TriggeredData& trigger_data);
//                    CHAOS_TRIGGER_CONSUMER_CLOSE_DEFINITION()
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__A1E8B6A_044F_48C2_B53B_93FBD4647091_DatasetConsumer_h */

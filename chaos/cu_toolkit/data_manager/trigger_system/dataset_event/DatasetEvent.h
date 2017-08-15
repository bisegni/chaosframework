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

#ifndef __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h
#define __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h

#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/common/trigger/AbstractEvent.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/DatasetSubject.h>
#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/dataset_event_types.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                namespace dataset_event {
                    namespace chaos_data = chaos::common::data;
                    namespace chaos_trigger = chaos::common::trigger;

                    CHAOS_TRIGGER_EVENT_OPEN_DESCRIPTION(EventDSAttributePreChange,
                                                         "Signal for pre change attribute value",
                                                         ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeTypePreChange,
                                                         chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType,
                                                         DatasetSubject)
                    //CHAOS_TRIGGER_EVENT_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
                    CHAOS_TRIGGER_EVENT_CLOSE_DESCRIPTION()
                    
                    CHAOS_TRIGGER_EVENT_OPEN_DESCRIPTION(EventDSAttributePostChange,
                                                         "Signal for post change attribute value",
                                                         ::chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeTypePostChange,
                                                         chaos::cu::data_manager::trigger_system::dataset_event::ETDatasetAttributeType,
                                                         DatasetSubject)
                    //CHAOS_TRIGGER_EVENT_ADD_PROPERTY("offset", "amount of int's added to 1", chaos::DataType::TYPE_INT32);
                    CHAOS_TRIGGER_EVENT_CLOSE_DESCRIPTION()
                    
                }
            }
        }
    }
}
#endif /* __CHAOSFramework__A590BCD_051A_4197_B57E_AE8E49060FB7_DatasetEvent_h */

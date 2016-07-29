/*
 *	EventConsumerDatasetChange.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 29/07/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__F658651_325A_4E8C_8657_CEF5771A520B_EventConsumerDatasetChange_h
#define __CHAOSFramework__F658651_325A_4E8C_8657_CEF5771A520B_EventConsumerDatasetChange_h

#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractEventType.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractEventConsumer.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                //! define the phases of the change attribute event
                typedef enum ETDatasetAttributeChangePhase {
                    ETDatasetAttributeChangePhasePre,
                    ETDatasetAttributeChangePhasePost
                }ETDatasetAttributeChangePhase;
                
                //! define the result of a attribute change event/consumer operation
                typedef enum ETDatasetAttributeChangeResult {
                    //!value has been accepted
                    ETDatasetAttributeChangeResultOK,
                    ETDatasetAttributeChangeResultRejected
                }ETDatasetAttributeChangeResult;
                
                
                
                //!define the event for attribute change
                class TriggerEventSetDSAttribute:
                public TriggerEvent {
                    ETDatasetAttributeChangePhase phase;
                public:
                    TriggerEventSetDSAttribute(const ETDatasetAttributeChangePhase phase);
                };
                
                
                //!define the abstract consumer for TriggerEventSetDSAttribute
                class EventConsumerSetDSAttribute:
                public AbstractEventConsumer {
                public:
                    EventConsumerSetDSAttribute();
                    ~EventConsumerSetDSAttribute();
                    virtual ETDatasetAttributeChangeResult executePre(ETDatasetAttributeChangePhase event_type,
                                                                      common::data::CDataVariant attribute_new_value,
                                                                      common::data::CDataVariant attribute_converted_value) = 0;
                    
                    virtual ETDatasetAttributeChangeResult executePost(ETDatasetAttributeChangePhase event_type,
                                                                       common::data::CDataVariant attribute_values) = 0;
                };

                //!Implement the consumer max/min check
                class EventConsumerSetDSAttributeMaxMin:
                public EventConsumerSetDSAttribute {
                public:
                    ETDatasetAttributeChangeResult executePre(ETDatasetAttributeChangePhase event_type,
                                                                      common::data::CDataVariant attribute_new_value,
                                                                      common::data::CDataVariant attribute_converted_value);
                    
                    ETDatasetAttributeChangeResult executePost(ETDatasetAttributeChangePhase event_type,
                                                                       common::data::CDataVariant attribute_values);
                };
                
                class EventConsumerSetDSAttributeTrigger {
                    
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__F658651_325A_4E8C_8657_CEF5771A520B_EventConsumerDatasetChange_h */

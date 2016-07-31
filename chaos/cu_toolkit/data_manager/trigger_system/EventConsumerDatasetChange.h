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

#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractEvent.h>
#include <chaos/cu_toolkit//data_manager/trigger_system/AbstractConsumer.h>

namespace chaos {
    namespace cu {
        namespace data_manager {
            namespace trigger_system {
                
                //! define the types for the event target dataset attribute
                typedef enum ETDatasetAttributeType {
                    ETDatasetAttributePreChange,
                    ETDatasetAttributePostChange
                }ETDatasetAttributeType;
                
                //! define the result of a attribute change event/consumer operation
                typedef enum ETDatasetAttributeResult {
                    //!value has been accepted
                    ETDatasetAttributeResultOK,
                    ETDatasetAttributeResultRejected
                }ETDatasetAttributeResult;
                
                
                
                //!define the event for attribute change
                class AbstractEventDSAttribute:
                public AbstractEvent {
                    ETDatasetAttributeType type;
                public:
                    AbstractEventDSAttribute(const std::string& _event_name,
                                             const ETDatasetAttributeType _type);
                    const ETDatasetAttributeType getType() const;
                };
                
                
                //!define the abstract consumer for AbstractEventSetDSAttribute
                class EventConsumerDSAttribute:
                public AbstractConsumer {
                public:
                    EventConsumerDSAttribute();
                    ~EventConsumerDSAttribute();
                    virtual ETDatasetAttributeResult execute(ETDatasetAttributeType event_type,
                                                                      common::data::CDataVariant attribute_new_value,
                                                                      common::data::CDataVariant attribute_converted_value) = 0;
                };

                //!Implement the consumer max/min check
                class EventConsumerDSAttributeMaxMin:
                public EventConsumerDSAttribute {
                public:
                    ETDatasetAttributeResult execute(ETDatasetAttributeType event_type,
                                                                  common::data::CDataVariant attribute_new_value,
                                                                  common::data::CDataVariant attribute_converted_value);
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__F658651_325A_4E8C_8657_CEF5771A520B_EventConsumerDatasetChange_h */

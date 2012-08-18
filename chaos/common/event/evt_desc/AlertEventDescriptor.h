/*
 *	AlertEventDescriptor.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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


#ifndef __CHAOSFramework__AlerEventDescriptor__
#define __CHAOSFramework__AlerEventDescriptor__
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>

namespace chaos {
    namespace  event {
        namespace alert {
            
            typedef enum {
                EventAlertThresholdCrossing = 0,
                EventAlertThresholdReset,
                EventAlertLastCodeNumber = EventAlertThresholdReset
            } EventAlertCode;
            
                //! Alert event type code
#define EVT_TYPE_CODE                 0
                //! max priority for alert
#define EVT_ALERT_DEFAULT_PRIORITY    63
                //! offset for the code
#define EVT_ALERT_CODE_OFFSET         5
                //! offset for the priority
#define EVT_ALERT_PRORITY_OFFSET      7
                //! offset for the data type
#define EVT_ALERT_DATA_TYPE_OFFSET    8
                //! offset for the data
#define EVT_ALERT_DATA_OFFSET         9
            
                //REGISTER_AND_DEFINE_NOALIAS_DERIVED_CLASS_FACTORY(AlertEventDescriptor,EventDescriptor)  {
            class AlertEventDescriptor : public EventDescriptor {
            protected:
                    //!Set the code of the alert
                /*!
                 Set the allert code for this event
                 \param alertCode thecode of the alert
                 */
                inline void setAlertCode(EventAlertCode alertCode);
                
                    //!Set a custom code in this event
                /*!
                 Set an allert custom code for this event. A custom code is a value,
                 after the last EventAlertCode values, that identifie a custom code define
                 by developer user
                 \param customAlertCode the custom code of the alert
                 */
                inline void setAlertCustomCode(uint16_t customAlertCode);
                
                
                    //!Set the priority of this alert
                /*!
                 Set the allert priority
                 \param alertPriority priority
                 */
                inline void setAlertPriority(uint16_t alertPriority);
                
                /*!
                 Set the Value for the type
                 \param valueType the enumeration that descrive the type of the value with EventDataType constant
                 \param valuePtr a pointer to the value
                 \param valueSizethe size of the value
                 \return the length of the value
                 */
                inline uint16_t setValueWithType(EventDataType valueType, const void *valuePtr, uint16_t valueSize = 0);
                
            public:
                
                    //!Event data inizialization
                /*!
                 Initialize the data for the base descriptor competence. These are
                 only the header and base length (header only length)
                 */
                void initData();
                
                
                
                    //!Return the code of this alert
                /*!
                 Return the alert code identified bythis event
                 \return the code of the alert
                 */
                EventAlertCode getAlertCode();
                
                
                
                    //!Return the custom code of this alert
                /*!
                 Return the alert custom code identified by this event
                 \return the custom code of the alert
                 */
                uint16_t getAlertCustomCode();
                
                
                    //!Return the code of this alert
                /*!
                 Return the alert code identified bythis event
                 \return the code of the alert
                 */
                uint16_t getAlertPriority();
                
                /*
                 Return the type of the value
                 */
                EventDataType getValuesType();
                
                /*!
                 Set the Value for the type
                 \param valueType the enumeration that descrive the type of the value with EventDataType constant
                 \param valuePtr a pointer to the value
                 \param valueSizethe size of the value
                 */
                void setAlert(EventAlertCode alertCode, uint16_t priority, EventDataType valueType, const void *valuePtr, uint16_t valueSize = 0);
                
                /*!
                 Set the Value for the custom type
                 \param valueType the enumeration that descrive the type of the value with EventDataType constant
                 \param valuePtr a pointer to the value
                 \param valueSizethe size of the value
                 */
                void setCustomAlert(uint16_t alertCustomCode, uint16_t priority, EventDataType valueType, const void *valuePtr, uint16_t valueSize = 0);
                
                uint16_t getAlertValueSize();
                
                void getAlertValue(void *valuePtr, uint16_t valueSize);
            };
        }
    }
}


#endif /* defined(__CHAOSFramework__AlerEventDescriptor__) */

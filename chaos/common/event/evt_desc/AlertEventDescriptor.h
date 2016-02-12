/*
 *	AlertEventDescriptor.h
 *	!CHAOS
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
    namespace common {
        namespace  event {
            namespace alert {
                
                typedef enum {
                    EventAlertThresholdCrossing = 0,
                    EventAlertThresholdReset,
                    EventAlertLogSubmitted,
                    EventAlertLastCodeNumber = EventAlertThresholdReset
                } EventAlertCode;
                
                //! max priority for alert
#define EVT_ALERT_DEFAULT_PRIORITY    63
                
                
                //REGISTER_AND_DEFINE_NOALIAS_DERIVED_CLASS_FACTORY(AlertEventDescriptor,EventDescriptor)  {
                class AlertEventDescriptor :
                public EventDescriptor {
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
                    
                public:
                    
                    AlertEventDescriptor();
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
                    
                    /*!
                     Set the Value for the type
                     \param valueType the enumeration that descrive the type of the value with EventDataType constant
                     \param valuePtr a pointer to the value
                     \param valueSizethe size of the value
                     */
                    void setAlert(const std::string& indetifier,
                                  uint16_t alertCode,
                                  uint16_t priority,
                                  EventDataType valueType,
                                  const void *valuePtr,
                                  uint16_t valueSize = 0);
                    
                    /*!
                     Set the Value for the custom type
                     \param valueType the enumeration that descrive the type of the value with EventDataType constant
                     \param valuePtr a pointer to the value
                     \param valueSizethe size of the value
                     */
                    void setCustomAlert(const std::string& indetifier,
                                        uint16_t alertCustomCode,
                                        uint16_t priority,
                                        EventDataType valueType,
                                        const void *valuePtr,
                                        uint16_t valueSize = 0);
                };
            }
        }
    }
}


#endif /* defined(__CHAOSFramework__AlerEventDescriptor__) */

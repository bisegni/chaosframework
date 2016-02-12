/*
 *	InstrumentEventDescriptor.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 19/08/12.
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

/*
 Intellectual property of   Copyright (c) 2012 INFN. All rights reserved.
 */

#ifndef __CHAOSFramework__InstrumentEventDescriptor__
#define __CHAOSFramework__InstrumentEventDescriptor__
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/event/evt_desc/EventDescriptor.h>
#include <chaos/common/exception/exception.h>

#define EVT_INSTRUMENT_DEFAULT_PRIORITY    31

namespace chaos {
    namespace common {
        namespace event{
            namespace instrument {
                
                //!Instrument event type code
                /*!
                 These represent the code for event type definition used
                 to define what happen into an instrument
                 */
                typedef enum {
                    //!The initialization pahse has been successfully done
                    EventInstrumentInitialized = 0,
                    //!The deinitialization pahse has been successfully done
                    EventInstrumentDeinitialized,
                    //!The instrument has been successfully started
                    EventInstrumentStarted,
                    //!The instrument has been successfully stopped
                    EventInstrumentStopped,
                    //! the intervall between run method call has been successfully changed
                    EventInstrumentNewScheduleDelay,
                    //!notify the change of some input attribute of the dataset
                    EventInstrumentInputDatasetAttributeChanged,
                    //!notify the heartbeat of insturment
                    EventInstrumentHeartbeat,
                    //! last entry
                    EventAlertLastCodeNumber = EventInstrumentHeartbeat
                } EventInstrumentCode;
                
                /*!
                 This class represent the definition for the instrumens event
                 */
                class InstrumentEventDescriptor : public EventDescriptor {
                public:
                    InstrumentEventDescriptor();
                    //! Define the event for the update scehdule time notification
                    void setNewScheduleDelay(const std::string&  instrumentID, uint64_t newValue);
                    //! Define the event for the input attribute dataset change scehdule time notification
                    void setDatasetInputAttributeChanged(const std::string&  instrumentID, uint16_t errorCode);
                    //! Define the event for the heartbeat of the instrument
                    void setEartbeat(const std::string&  instrumentID);
                    /*!
                     Set the Value for the type
                     \param valueType the enumeration that descrive the type of the value with EventDataType constant
                     \param valuePtr a pointer to the value
                     \param valueSizethe size of the value
                     */
                    void setInstrument(const std::string& indetifier,
                                       uint16_t alert_code,
                                       uint16_t priority,
                                       EventDataType value_type,
                                       const void *value_ptr,
                                       uint16_t value_size = 0) throw (CException);
                };
                
            }
        }
    }
    
}
#endif /* defined(__CHAOSFramework__InstrumentEventDescriptor__) */

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
                                       uint16_t value_size = 0) ;
                };
                
            }
        }
    }
    
}
#endif /* defined(__CHAOSFramework__InstrumentEventDescriptor__) */

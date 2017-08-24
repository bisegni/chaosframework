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

#ifndef __CHAOSFramework__DeviceEventChannel__
#define __CHAOSFramework__DeviceEventChannel__

#include <chaos/common/event/channel/EventChannel.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
namespace chaos {
    namespace common {
        namespace network {
            class NetworkBroker;
        }
        namespace event {
            namespace channel {
                
                //! Channel for manage the alert event type
                /*!
                 This channel simplify the alert event forwarding and recivement
                 */
                class InstrumentEventChannel : public EventChannel {
                    friend class chaos::common::network::NetworkBroker;
                protected:
                    
                    InstrumentEventChannel(chaos::common::network::NetworkBroker *rootBroker);
                    
                    ~InstrumentEventChannel();
                    
                    void handleEvent(const event::EventDescriptor * const event);
                    
                    int sendEvent(const std::string& identification,
                                  uint16_t sub_code,
                                  uint16_t priority,
                                  EventDataType type_of_data,
                                  const void *value_ptr,
                                  uint16_t value_size = 0);
                public:
                    
                    //--------------------inherited-----------------
                    void activateChannelEventReception(EventAction *event_action);
                    
                    //! Send an event for the scehdule update setting
                    /*!
                     Take care to construct and send an instrument event forn the schedule update
                     \param insturmentID the identificatin Of the instrument
                     \param newScheduleUpdateTime new run scehdule delay value
                     */
                    int notifyForScheduleUpdateWithNewValue(const char * insturmentID, uint64_t newScheduleUpdateTime);
                    
                    //! Send an event for the dataset attribute setting
                    /*!
                     Notify that the setting of some input attribute of the dataset hase been done
                     \param insturmentID the indetificaiton of instrument
                     \param error gived settin the param
                     */
                    int notifyForAttributeSetting(const char * insturmentID, uint16_t error);
                    
                    //! Send an event for the heartbeat
                    /*!
                     \param insturmentID the indetificaiton of instrument
                     */
                    int notifyHeartbeat(const char * insturmentID);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceEventChannel__) */

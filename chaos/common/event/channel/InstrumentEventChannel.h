/*
 *	InstrumentEventChannel.h
 *	CHAOSFramework
 *	Created by Claudio Bisegni on 02/09/12.
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

#ifndef __CHAOSFramework__DeviceEventChannel__
#define __CHAOSFramework__DeviceEventChannel__

#include <chaos/common/event/channel/EventChannel.h>
#include <chaos/common/event/evt_desc/AlertEventDescriptor.h>
namespace chaos {
    
    class NetworkBroker;
    
    namespace event {
        namespace channel {
            
                //! Channel for manage the alert event type
            /*!
             This channel simplify the alert event forwarding and recivement
             */
            class InstrumentEventChannel : public EventChannel {
                friend class chaos::NetworkBroker;
            protected:
                
                InstrumentEventChannel(NetworkBroker *rootBroker);
                
                ~InstrumentEventChannel();
                
                void handleEvent(const event::EventDescriptor * const event);
                
                int sendEvent(const char * const identificationString, uint16_t subCode, uint16_t priority, EventDataType typeOfData, const void *valuePtr, uint16_t valueSize = 0);
            public:
                
                    //--------------------inherited-----------------
                void activateChannelEventReception();
                
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

#endif /* defined(__CHAOSFramework__DeviceEventChannel__) */

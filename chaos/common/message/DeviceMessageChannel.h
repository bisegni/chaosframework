/*
 *	DeviceMessageChannel.h
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
#ifndef CHAOSFramework_DeviceMessageChannel_h
#define CHAOSFramework_DeviceMessageChannel_h

#include <string>
#include <chaos/common/message/NodeMessageChannel.h>
#include <chaos/common/network/CNodeNetworkAddress.h>

namespace chaos {
	namespace common {
		namespace message {
			using namespace std;
			
			//! Message Channel specialize for metadataserver comunication
			/*!
			 This class represent a message chanel for comunication with a device
			 */
			class DeviceMessageChannel :
            public NodeMessageChannel {
				friend class chaos::common::network::NetworkBroker;
				CDeviceNetworkAddress *deviceNetworkAddress;
				
			protected:
				//! base constructor
				/*!
				 The constructor create a channel for comunicate with the device that is contained in a Contro Unit, so the full network address is
				 ip:port:cu_node_address(instance):deviceID
				 */
				DeviceMessageChannel(NetworkBroker *msgBroker, CDeviceNetworkAddress *_deviceNetworkAddress);
				
			public:
				void setNewAddress(CDeviceNetworkAddress *_deviceAddress);
				//!Initialization of the device
				/*!
				 Perform the hardware initialization
				 \param initData the pointer to the data for device initialization. The memory is not deallocate after the send operation
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int initDevice(common::data::CDataWrapper *initData, uint32_t millisecToWait = 0);
				//!Device deinititalization
				/*!
				 Perform the hardware deinitialization
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int deinitDevice(uint32_t millisecToWait = 0);
				//!Start the device
				/*!
				 Perform the hardware start of scheduling
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int startDevice(uint32_t millisecToWait = 0);
				//! Stop the device
				/*!
				 Perform the hardware stop of the scheduling
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int stopDevice(uint32_t millisecToWait = 0);
				//! Restore the device to a tag
				/*!
				 Perform the restore of the device state as was at tag
				 \param restore_tag the alias of the saved tag
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int restoreDeviceToTag(const std::string& restore_tag, uint32_t millisecToWait = 0);
				//! get type fo the control unit
				/*!
				 Perform request for get the information of the control unit
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int getType(std::string& control_unit_type, uint32_t millisecToWait = 0);
				//! Get device state
				/*!
				 Get the current state of the hardware
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int getState(CUStateKey::ControlUnitState& deviceState, uint32_t millisecToWait = 0);
				
				//! Send the CDatawrapper as device attribute values
				/*!
				 All the CDataWrapper is sent as pack for the device attributes values, no check is done
				 \param attributesValues the container for the values of some dataset attributes
				 */
				int setAttributeValue(common::data::CDataWrapper& attributesValues, bool noWait = false, uint32_t millisecToWait = 0);
				
				//!Set the device thread schedule delay
				/*!
				 Set the delay between a schedule and the next
				 */
				int setScheduleDelay(uint64_t scheduledDealy, uint32_t millisecToWait = 0);
				
				/*!
				 \brief send a message to a custom action
				 */
				void sendCustomMessage(const std::string& action_name,
                                       common::data::CDataWrapper* const,
                                       bool queued = true);
				
				/*!
				 \brief send a request to a custom action
				 */
				int sendCustomRequest(const std::string& action_name,
                                      common::data::CDataWrapper* const,
                                      common::data::CDataWrapper** result_data,
                                      uint32_t millisec_to_wait = 0,
                                      bool async = false,
                                      bool queued = true);
                
                /*!
                 send a request to a custom action
                 */
                std::auto_ptr<MessageRequestFuture>  sendCustomRequestWithFuture(const std::string& action_name,
                                                                                 common::data::CDataWrapper *request_data);
			};
		}
	}
}
#endif

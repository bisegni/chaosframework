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
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/NodeMessageChannel.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/async_central/async_central.h>

namespace chaos {
	namespace common {
		namespace message {
			using namespace std;
			
			//! Message Channel specialize for metadataserver comunication
			/*!
			 This class represent a message chanel for comunication with a device
			 */
			class DeviceMessageChannel :
            public NodeMessageChannel,
            public chaos::common::async_central::TimerHandler {
				friend class chaos::common::network::NetworkBroker;
                
                //!managment variable
                bool online;
                bool self_managed;
                bool auto_reconnection;

                MDSMessageChannel *local_mds_channel;
				CDeviceNetworkAddress *device_network_address;

                void setOnline(bool new_online_state);
                void tryToReconnect();
			protected:
				//! base constructor
				/*!
				 The constructor create a channel for comunicate with the device that is contained in a Contro Unit, so the full network address is
				 ip:port:cu_node_address(instance):deviceID
				 */
				DeviceMessageChannel(NetworkBroker *msgBroker,
                                     CDeviceNetworkAddress *_device_network_address,
                                     bool _self_managed = false,
                                     MessageRequestDomainSHRDPtr _new_message_request_domain = MessageRequestDomainSHRDPtr(new MessageRequestDomain()));

                    //!inherited method
                void timeout();
                /*!
                 Initialization phase of the channel
                 */
                virtual void init() throw(CException);
                
                /*!
                 Initialization phase of the channel
                 */
                virtual void deinit() throw(CException);
			public:
                    //!Update the network address of the node
                /*!
                 fetch from mds the network address and domain of this device, in case
                 it is changed it uopdate the local address and try to ping to new, if
                 all goes ok the status is put on Online.
                 \return true if something on address is changed
                 */
                bool udpateNetworkAddress(int32_t millisec_to_wait = 1000);
                
                    //!return true if devce is online
                bool isOnline();

                    //!update the adress of the device
				void setNewAddress(CDeviceNetworkAddress *_deviceAddress);

                    //! set channel in auto reconnection mode
                /*!
                 in this mode a timer is started when the device is no mode
                 on line, every fire of that timer a query is done to the 
                 remote rpc domain f the device to check if it is online.
                 */
                void setAutoReconnection(bool auto_reconnection);
                
                //! set the channel as auto managed
                /*!
                 Setting auto_managed property to true, channel create internally
                 an new MDS Channel and share with it a request domain. At this point
                 can be used the auto_reconnection feature. In this way DeviceMessageChannel can
                 obtain address of the device id also if it change.
                 this property need a reinitilization after it is changed.
                 */
                void setSelfManaged(bool _auto_managed);
                
				//!Initialization of the device
				/*!
				 Perform the hardware initialization
				 \param initData the pointer to the data for device initialization. The memory is not deallocate after the send operation
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int initDevice(common::data::CDataWrapper *initData, int32_t millisec_to_wait = 1000);

                    //!Initialization of the device
                /*!
                 Perform initilizzation of the control unit getting the default 
                 setting from mds with internal channel
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int initDeviceToDefaultSetting(int32_t millisec_to_wait = 1000);

				//!Device deinititalization
				/*!
				 Perform the hardware deinitialization
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int deinitDevice(int32_t millisec_to_wait = 1000);
				//!Start the device
				/*!
				 Perform the hardware start of scheduling
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int startDevice(int32_t millisec_to_wait = 1000);
				//! Stop the device
				/*!
				 Perform the hardware stop of the scheduling
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int stopDevice(int32_t millisec_to_wait = 1000);
                                
                                /*!
                                    Recover from recoverable error
                                 * \millisecToWait the number of millisecond for waith the answer
				 */
				int recoverDeviceFromError(int32_t millisec_to_wait = 1000);
				//! Restore the device to a tag
				/*!
				 Perform the restore of the device state as was at tag
				 \param restore_tag the alias of the saved tag
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int restoreDeviceToTag(const std::string& restore_tag, int32_t millisec_to_wait = 1000);
				//! get type fo the control unit
				/*!
				 Perform request for get the information of the control unit
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int getType(std::string& control_unit_type, int32_t millisec_to_wait = 1000);
				//! Get device state
				/*!
				 Get the current state of the hardware
				 \millisecToWait the number of millisecond for waith the answer
				 */
				int getState(CUStateKey::ControlUnitState& deviceState, int32_t millisec_to_wait = 1000);
				
				//! Send the CDatawrapper as device attribute values
				/*!
				 All the CDataWrapper is sent as pack for the device attributes values, no check is done
				 \param attributesValues the container for the values of some dataset attributes
				 */
				int setAttributeValue(common::data::CDataWrapper& attributesValues, bool noWait = false, int32_t millisec_to_wait = 1000);
				
				//!Set the device thread schedule delay
				/*!
				 Set the delay between a schedule and the next
				 */
				int setScheduleDelay(uint64_t scheduledDealy, int32_t millisec_to_wait = 1000);
				
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
                                      uint32_t millisec_to_wait = 1000,
                                      bool async = false,
                                      bool queued = true);
                
                /*!
                 send a request to a custom action
                 */
                std::auto_ptr<MessageRequestFuture>  sendCustomRequestWithFuture(const std::string& action_name,
                                                                                 common::data::CDataWrapper *request_data);
                
                //! Send a request for receive RPC information
                std::auto_ptr<MessageRequestFuture> checkRPCInformation();
                
                //! Send a request for an echo test
                std::auto_ptr<MessageRequestFuture> echoTest(chaos::common::data::CDataWrapper *echo_data);
			};
		}
	}
}
#endif

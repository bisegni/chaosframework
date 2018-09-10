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
#ifndef CHAOSFramework_DeviceMessageChannel_h
#define CHAOSFramework_DeviceMessageChannel_h

#include <string>

#include <chaos/common/chaos_types.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/message/NodeMessageChannel.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/common/async_central/async_central.h>
#include <chaos/common/utility/AbstractListenerContainer.h>

namespace chaos {
    namespace common {
        namespace message {
            using namespace std;
            //forward declaration
            class DeviceMessageChannel;
            
            typedef enum OnlineState {
                OnlineStateUnknown,
                OnlineStateOffline,
                OnlineStateOnline
            } OnlineState;
            
            //! status flag listener
            class DeviceMessageChannelListener:
            public chaos::common::utility::AbstractListener {
                friend class DeviceMessageChannel;
            public:
                //!signal the change of the current selected level severity
                virtual void deviceAvailabilityChanged(const std::string& device_id,
                                                       const OnlineState availability) = 0;
            };
            
            
            //! Message Channel specialize for metadataserver comunication
            /*!
             This class represent a message chanel for comunication with a device
             */
            class DeviceMessageChannel:
            public NodeMessageChannel,
            public chaos::common::async_central::TimerHandler,
            protected chaos::common::utility::AbstractListenerContainer {
                friend class chaos::common::network::NetworkBroker;
                
                bool self_managed;
                bool auto_reconnection;
                
                OnlineState online;
                ChaosSharedMutex mutex_online_state;
                int online_retry;
                MDSMessageChannel *local_mds_channel;
                CDeviceNetworkAddress *device_network_address;
                
                void setOnline(OnlineState new_online_state);
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
                virtual void init();
                
                /*!
                 Initialization phase of the channel
                 */
                virtual void deinit();
                
                //! request result introspection
                /*!
                 perform an introspection into the result of a request
                 to determinate if the channel still is online
                 */
                void requestPromisesHandler(const FuturePromiseData& response_data);
                
                //customize abstract handler implementation for fire devi ce channel listener event
                void fireToListener(unsigned int fire_code,
                                    chaos::common::utility::AbstractListener *listener_to_fire);
            public:
                using NodeMessageChannel::echoTest;
                
                const std::string& getDeviceID() const;
                //!Update the network address of the node
                /*!
                 fetch from mds the network address and domain of this device, in case
                 it is changed it uopdate the local address and try to ping to new, if
                 all goes ok the status is put on Online.
                 \return true if something on address is changed
                 */
                bool udpateNetworkAddress(int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                //!Add new listener
                void addListener(DeviceMessageChannelListener *new_listener);
                
                //!Remove new listener
                void removeListener(DeviceMessageChannelListener *listener);
                
                //!return true if devce is online
                OnlineState isOnline();
                
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
                int initDevice(chaos::common::data::CDWUniquePtr init_data,
                               int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                //!Initialization of the device
                /*!
                 Perform initilizzation of the control unit getting the default
                 setting from mds with internal channel
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int initDeviceToDefaultSetting(int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                //!Device deinititalization
                /*!
                 Perform the hardware deinitialization
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int deinitDevice(int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                //!Start the device
                /*!
                 Perform the hardware start of scheduling
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int startDevice(int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                //! Stop the device
                /*!
                 Perform the hardware stop of the scheduling
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int stopDevice(int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                /*!
                 Recover from recoverable error
                 * \millisecToWait the number of millisecond for waith the answer
                 */
                int recoverDeviceFromError(int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                //! Restore the device to a tag
                /*!
                 Perform the restore of the device state as was at tag
                 \param restore_tag the alias of the saved tag
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int restoreDeviceToTag(const std::string& restore_tag,
                                       int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                //! get type fo the control unit
                /*!
                 Perform request for get the information of the control unit
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int getType(std::string& control_unit_type,
                            int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                //! Get device state
                /*!
                 Get the current state of the hardware
                 \millisecToWait the number of millisecond for waith the answer
                 */
                int getState(CUStateKey::ControlUnitState& deviceState,
                             int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                //! Send the CDatawrapper as adevice attribute values
                /*!
                 All the CDataWrapper is sent as pack for the device attributes values, no check is done
                 \param attributesValues the container for the values of some dataset attributes
                 */
                int setAttributeValue(chaos::common::data::CDWUniquePtr attribute_values,
                                      bool noWait,
                                      int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                //!Set the device thread schedule delay
                /*!
                 Set the delay between a schedule and the next
                 */
                int setScheduleDelay(uint64_t scheduledDealy, int32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                /*!
                 \brief send a message to a custom action
                 */
                int sendCustomMessage(const std::string& action_name,
                                      chaos::common::data::CDWUniquePtr message_data);
                
                /*!
                 \brief send a request to a custom action
                 */
                int sendCustomRequest(const std::string& action_name,
                                      chaos::common::data::CDWUniquePtr message_data,
                                      chaos::common::data::CDWUniquePtr& result_data,
                                      uint32_t millisec_to_wait = RpcConfigurationKey::GlobalRPCTimeoutinMSec);
                
                /*!
                 send a request to a custom action
                 */
                ChaosUniquePtr<MessageRequestFuture>  sendCustomRequestWithFuture(const std::string& action_name,
                                                                                  chaos::common::data::CDWUniquePtr request_data);
                
                //! Send a request for receive RPC information
                ChaosUniquePtr<MessageRequestFuture> checkRPCInformation();
            };
        }
    }
}
#endif

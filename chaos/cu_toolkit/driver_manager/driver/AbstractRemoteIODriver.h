/*
 *	AbstractRemoteIODriver.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 19/07/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h
#define __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <chaos/cu_toolkit/external_gateway/ExternalUnitEndpoint.h>

#include <chaos/common/async_central/async_central.h>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //! define the abstraction fo comunicate with a remote driver
                /*!
                 Remote driver is a !CHAOS feature taht permit to a remote process to connect interact with
                 a control unit using an http protocols and work as a driver for that control unit
                 
                 the messages sent to the driver has the follow json scheme:
                 request:
                 {
                 }
                 */
                class AbstractRemoteIODriver:
                ADD_CU_DRIVER_PLUGIN_SUPERCLASS,
                chaos::cu::external_gateway::ExternalUnitEndpoint,
                chaos::common::async_central::TimerHandler{
                    
                    typedef ChaosPromise< chaos::common::data::CDWShrdPtr > DriverResultPromise;
                    typedef ChaosFuture< chaos::common::data::CDWShrdPtr > DriverResultFuture;
                    
                    struct DriverResultInfo {
                        uint64_t request_ts;
                        DriverResultPromise promise;
                    };
                    typedef ChaosSharedPtr< DriverResultInfo > DriverResultInfoShrdPtr;
                    
                    CHAOS_DEFINE_MAP_FOR_TYPE(uint64_t,  DriverResultInfoShrdPtr, MapResult);
                    CHAOS_DEFINE_LOCKABLE_OBJECT(MapResult, LMapResult);
                    CHAOS_DEFINE_LOCKABLE_OBJECT(std::string, LString);
                    
                    ChaosAtomic<uint64_t> message_counter;
                    
                    //contains the autorization key that need to be passed by the remote driver
                    std::string authorization_key;
                    
                    LString     current_connection_identifier;
                    LMapResult  map_of_promises;
                    
                    void driverInit(const char *initParameter) throw (chaos::CException);
                    void driverDeinit() throw (chaos::CException);
                public:
                    //!define the type of the message that can be forw
                    typedef enum {
                        MessageTypeDatasetRequest = 0,
                        MessageTypeWrite,
                        MessageTypeRead
                    } MessageType;
                    
                    typedef enum {
                        AR_ERROR_OK = 0,
                        AR_ERROR_NO_CONNECTION,
                        AR_ERROR_TIMEOUT,
                        AR_ERROR_INVALID_MESSAGE_TYPE
                    } AR_ERROR;
                    
                    AbstractRemoteIODriver();
                    
                    ~AbstractRemoteIODriver();
                    
                    //!Request the dataset to the driver
                    /*!
                     Message is composed and sent to the remote driver. The answer is wait
                     for the maximum number of milliseconds the data receive returned
                     */
                    int getDriverDataset(chaos::common::data::CDWUniquePtr& received_data);
                    
                    //!read a variable from the driver
                    /*!
                     Message is composed to the read operation and sent to the driver. The response is wait for the maximum
                     number of milliseconds and returned (if one).
                     \param variable name of variable
                     \param value filled with the value read from the remote driver
                     */
                    int readVariable(const std::string& variable,
                                     const chaos::common::data::CDataVariant& value);
                    
                    //!write a variable from the driver
                    /*!
                     Message is composed to the write operation and sent to the driver. No answer is wait
                     \param variable name of variable
                     \param value to send to the remote driver to be stored in variable
                     */
                    int writeVariable(const std::string& variable,
                                      const chaos::common::data::CDataVariant& value);
                    
                    //!Send raw request to the remote driver
                    /*!
                     \param message_type is the type of the raw message to be sent to remote driver
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendRawRequest(MessageType message_type,
                                       chaos::common::data::CDWUniquePtr message_data,
                                       AbstractRemoteIODriver::DriverResultFuture& request_future);
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_type is the type of the raw message to be sent to remote driver
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    int sendRawMessage(MessageType message_type,
                                       chaos::common::data::CDWUniquePtr message_data);
                protected:
                    //!inherited method by @ExternalUnitEndpoint
                    void handleNewConnection(const std::string& connection_identifier);
                    //!inherited method by @ExternalUnitEndpoint
                    void handleDisconnection(const std::string& connection_identifier);
                    
                    int handleReceivedeMessage(const std::string& connection_identifier,
                                               ChaosUniquePtr<chaos::common::data::CDataWrapper> message);
                    
                    void composeRequest(chaos::common::data::CDWUniquePtr& ext_msg,
                                        uint64_t request_index,
                                        MessageType message_type,
                                        chaos::common::data::CDWUniquePtr& message_data);
                    void composeMessage(chaos::common::data::CDWUniquePtr& ext_msg,
                                        MessageType message_type,
                                        chaos::common::data::CDWUniquePtr& message_data);
                    //!inherited from chaos::common::async_central::TimerHandler
                    void timeout();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h */

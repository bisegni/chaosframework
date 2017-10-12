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

#ifndef __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h
#define __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <chaos/common/external_unit/ExternalUnitServerEndpoint.h>

#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/thread/FutureHelper.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //! define the abstraction fo comunicate with a remote driver
                /*!
                 Remote driver is a !CHAOS feature taht permit to a remote process to connect interact with
                 a control unit using an http protocols and work as a driver for that control unit
                 
                 the messages sent to the driver has the follow json scheme:
                 autorization:
                 {
                 authorization_key:string vlaue
                 .....
                 }
                 
                 request:
                 {
                 request_id:int32
                 .....
                 }
                 response:
                 {
                 request_id:int32 <--- the same of the answer
                 .....
                 }
                 
                 message: //not have the request field
                 {
                 .....
                 }
                 */
                typedef chaos::common::thread::FutureHelper<chaos::common::data::CDWShrdPtr> CDWShrdPtrFutureHelper;

                
                class AbstractRemoteIODriver:
                ADD_CU_DRIVER_PLUGIN_SUPERCLASS,
                chaos::common::external_unit::ExternalUnitServerEndpoint,
                protected CDWShrdPtrFutureHelper {
                    //!initialization and deinitialization driver methods
                    void driverInit(const char *initParameter) throw (chaos::CException);
                    void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException);
                    void driverDeinit() throw (chaos::CException);
                public:
                    
                    //!liste of error
                    typedef enum {
                        AR_ERROR_OK = 0,
                        AR_ERROR_NO_CONNECTION,
                        AR_ERROR_NOT_AUTORIZED,
                        AR_ERROR_TIMEOUT,
                        AR_ERROR_LAST = 100
                    } AR_ERROR;
                    
                    AbstractRemoteIODriver();
                    
                    ~AbstractRemoteIODriver();
                    
                    //!Send raw request to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = 5000);
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    int sendRawMessage(chaos::common::data::CDWUniquePtr message_data);
                    
                    //!keep public these method
                    using ExternalUnitServerEndpoint::setNumberOfAcceptedConnection;
                    using ExternalUnitServerEndpoint::getNumberOfAcceptedConnection;
                protected:
                    typedef enum {
                        RDConnectionPhaseDisconnected,
                        RDConnectionPhaseConnected,
                        RDConnectionPhaseAutorized,
                    } RDConnectionPhase;
                    //! handle called when a new message has been received
                    /*!
                     A new message has been received from the rmeote server in an asyc way. It
                     will be forward to sublcass only if remote server has been successfully registered
                     */
                    virtual int asyncMessageReceived(chaos::common::data::CDWUniquePtr message) = 0;
                    
                    //!send an errore to remote driver
                    int sendError(int error_code,
                                   std::string& error_message,
                                   std::string& error_domain);
                private:
                    CHAOS_DEFINE_LOCKABLE_OBJECT(std::string, LString);
                    LString             current_connection_identifier;
                    std::string         authorization_key;
                    RDConnectionPhase   conn_phase;
                    
                    //!inherited method by @ExternalUnitEndpoint
                    void handleNewConnection(const std::string& connection_identifier);
                    //!inherited method by @ExternalUnitEndpoint
                    void handleDisconnection(const std::string& connection_identifier);
                    
                    int handleReceivedeMessage(const std::string& connection_identifier,
                                               ChaosUniquePtr<chaos::common::data::CDataWrapper> message);
                    
                    //!
                    void sendAuthenticationACK();
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h */

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
#include <chaos/common/external_unit/external_unit.h>
#include <chaos/common/external_unit/ExternalUnitServerEndpoint.h>

#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/thread/FutureHelper.h>
#include <chaos/common/utility/LockableObject.h>
#include <chaos/common/async_central/async_central.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#define AUTHORIZATION_KEY        "authorization_key"
#define AUTHORIZATION_STATE      "authorization_state"
#define MESSAGE                 "message"
#define REQUEST_IDENTIFICATION  "request_id"

#define INFO    INFO_LOG(AbstractCDataWrapperIODriver)
#define DBG        DBG_LOG(AbstractCDataWrapperIODriver)
#define ERR        ERR_LOG(AbstractCDataWrapperIODriver)

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //!liste of error
                typedef enum {
                    AR_ERROR_OK = 0,
                    AR_ERROR_NO_CONNECTION,
                    AR_ERROR_NOT_AUTORIZED,
                    AR_ERROR_TIMEOUT,
                    AR_ERROR_LAST = 100
                } AR_ERROR;
                
                typedef chaos::common::thread::FutureHelper<chaos::common::data::CDWShrdPtr> CDWShrdPtrFutureHelper;
                
                template<typename EndpointType>
                class AbstractRemoteIODriver:
                public chaos::cu::driver_manager::driver::AbstractDriver,
                public chaos::cu::driver_manager::driver::AbstractDriverPlugin,
                public EndpointType,
                protected CDWShrdPtrFutureHelper {
                protected:
                    //!initialization and deinitialization driver methods
                    void driverInit(const char *initParameter) throw (chaos::CException) {
                        INFO << "Init driver:"<<initParameter;
                        CHECK_ASSERTION_THROW_AND_LOG(isDriverParamInJson(), ERR, -1, "Init parameter need to be formated in a json document");
                        Json::Value root_param_document = getDriverParamJsonRootElement();
                        Json::Value jv_authorization_key = root_param_document[AUTHORIZATION_KEY];
                        CHECK_ASSERTION_THROW_AND_LOG((jv_authorization_key.isNull() == false), ERR, -3, "The authorization key is mandatory");
                        
                        if(jv_authorization_key.isNull() == false) {
                            authorization_key = jv_authorization_key.asString();
                        }
                        
                        CHECK_ASSERTION_THROW_AND_LOG(authorization_key.size(), ERR, -4, "The authorization key cannot be zero lenght");
                        
                        CDWShrdPtrFutureHelper::init(NULL);
                        }
                        void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
                            CHECK_ASSERTION_THROW_AND_LOG((init_parameter.isEmpty() == false), ERR, -1, "Init parameter need to be formated in a json document");
                            CHECK_ASSERTION_THROW_AND_LOG(init_parameter.hasKey(AUTHORIZATION_KEY), ERR, -3, "The authorization key is mandatory")
                            //get the authorization key
                            authorization_key = init_parameter.getStringValue(AUTHORIZATION_KEY);
                            CHECK_ASSERTION_THROW_AND_LOG(authorization_key.size(), ERR, -4, "The authorization key cannot be zero lenght");
                            
                            CDWShrdPtrFutureHelper::init(NULL);
                        }
                        void driverDeinit() throw (chaos::CException) {
                            INFO << "Deinit driver";
                            CHAOS_NOT_THROW(CDWShrdPtrFutureHelper::deinit();)
                        }
                    public:
                        
                        AbstractRemoteIODriver():
                        chaos::cu::driver_manager::driver::AbstractDriverPlugin(this),
                        EndpointType(),
                        authorization_key(),
                        conn_phase(RDConnectionPhaseDisconnected){}
                        
                        ~AbstractRemoteIODriver(){}
                        
                        //!Send raw request to the remote driver
                        /*!
                         \param message_data is the raw data to be transmitted to the remote driver
                         \param received_data si the raw data received from the driver
                         */
                        int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                           chaos::common::data::CDWShrdPtr& message_response,
                                           uint32_t timeout = 5000) {
                            LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                            switch(conn_phase) {
                                case RDConnectionPhaseDisconnected:
                                    return AR_ERROR_NO_CONNECTION;
                                    break;
                                case RDConnectionPhaseConnected:
                                    CHAOS_ASSERT(current_connection_identifier().size());
                                    return AR_ERROR_NOT_AUTORIZED;
                                    break;
                                case RDConnectionPhaseAutorized:
                                    //we can proceeed
                                    break;
                            }
                            CDWShrdPtrFutureHelper::CounterType new_promise_id;
                            CDWShrdPtrFutureHelper::Future request_future;
                            chaos::common::data::CDWUniquePtr ext_msg(new chaos::common::data::CDataWrapper());
                            CDWShrdPtrFutureHelper::addNewPromise(new_promise_id,
                                                                  request_future);
                            
                            ext_msg->addInt32Value(REQUEST_IDENTIFICATION, new_promise_id);
                            ext_msg->addCSDataValue(MESSAGE, *message_data);
                            
                            //send message to driver
                            EndpointType::sendMessage(current_connection_identifier(),
                                                      ChaosMoveOperator(ext_msg));
                            ChaosFutureStatus f_status = request_future.wait_for(ChaosCronoMilliseconds(timeout));
                            if(f_status == ChaosFutureStatus::ready) {
                                message_response = request_future.get();
                                return AR_ERROR_OK;
                            } else {
                                return AR_ERROR_TIMEOUT;
                            }
                        }
                        
                        //!Send raw message to the remote driver
                        /*!
                         \param message_data is the raw data to be transmitted to the remote driver
                         */
                        int sendRawMessage(chaos::common::data::CDWUniquePtr message_data) {
                            LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                            switch(conn_phase) {
                                case RDConnectionPhaseDisconnected:
                                    return AR_ERROR_NO_CONNECTION;
                                    break;
                                case RDConnectionPhaseConnected:
                                    CHAOS_ASSERT(current_connection_identifier().size());
                                    return AR_ERROR_NOT_AUTORIZED;
                                    break;
                                case RDConnectionPhaseAutorized:
                                    //we can proceeed
                                    break;
                            }
                            
                            //send message to driver
                            EndpointType::sendMessage(current_connection_identifier(),
                                                      ChaosMoveOperator(message_data));
                            //we have connection
                            return AR_ERROR_OK;
                        }
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
                                      std::string& error_domain) {
                            LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                            switch(conn_phase) {
                                case RDConnectionPhaseDisconnected:
                                    return AR_ERROR_NO_CONNECTION;
                                    break;
                                case RDConnectionPhaseConnected:
                                    CHAOS_ASSERT(current_connection_identifier().size());
                                    return AR_ERROR_NOT_AUTORIZED;
                                    break;
                                case RDConnectionPhaseAutorized:
                                    //we can proceeed
                                    break;
                            }
                            EndpointType::sendError(current_connection_identifier(),
                                                    error_code, error_message, error_domain);
                            return AR_ERROR_OK;
                        }
                    private:
                        CHAOS_DEFINE_LOCKABLE_OBJECT(std::string, LString);
                        LString             current_connection_identifier;
                        std::string         authorization_key;
                        RDConnectionPhase   conn_phase;
                        
                        //!inherited method by @ExternalUnitEndpoint
                        void handleNewConnection(const std::string& connection_identifier){
                            LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                            current_connection_identifier() = connection_identifier;
                            conn_phase = RDConnectionPhaseConnected;
                        }
                        //!inherited method by @ExternalUnitEndpoint
                        void handleDisconnection(const std::string& connection_identifier) {
                            LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                            current_connection_identifier().clear();
                            conn_phase = RDConnectionPhaseDisconnected;
                        }
                        
                        int handleReceivedeMessage(const std::string& connection_identifier,
                                                   ChaosUniquePtr<chaos::common::data::CDataWrapper> message) {
                            if(conn_phase != RDConnectionPhaseAutorized) {
                                //check if a message with autorization key will arive
                                if(message->hasKey(AUTHORIZATION_KEY) &&
                                   message->isStringValue(AUTHORIZATION_KEY)) {
                                    if(authorization_key.compare(message->getStringValue(AUTHORIZATION_KEY)) == 0){
                                        conn_phase = RDConnectionPhaseAutorized;
                                        sendAuthenticationACK();
                                    } else {
                                        //send error because not right type of req index
                                        EndpointType::sendError(connection_identifier,
                                                                -1, "Authentication failed", __PRETTY_FUNCTION__);
                                        EndpointType::closeConnection(connection_identifier);
                                    }
                                }
                            } else {
                                if(!message->hasKey(MESSAGE)){
                                    //send error because not right type of req index
                                    EndpointType::sendError(connection_identifier,
                                                            -2, "message field is mandatory", __PRETTY_FUNCTION__);
                                } else if(!message->isCDataWrapperValue(MESSAGE)) {
                                    //send error because not right type of req index
                                    EndpointType::sendError(connection_identifier,
                                                            -3, "message field need to be an object type", __PRETTY_FUNCTION__);
                                } else if(!message->hasKey(REQUEST_IDENTIFICATION)) {
                                    asyncMessageReceived(ChaosMoveOperator(message));
                                } else if(!message->isInt32Value(REQUEST_IDENTIFICATION)) {
                                    //send error because not right type of req index
                                    EndpointType::sendError(connection_identifier,
                                                            -4, "request_id field need to be a int32 type", __PRETTY_FUNCTION__);
                                }  else {
                                    //good request index
                                    const uint32_t req_index = message->getUInt32Value(REQUEST_IDENTIFICATION);
                                    ChaosUniquePtr<chaos::common::data::CDataWrapper> embedded_message(message->getCSDataValue(MESSAGE));
                                    chaos::common::data::CDWShrdPtr message_shrd_ptr(embedded_message.release());
                                    CDWShrdPtrFutureHelper::setDataForPromiseID(req_index, message_shrd_ptr);
                                }
                            }
                            return 0;
                        }
                        
                        //!
                        void sendAuthenticationACK() {
                            chaos::common::data::CDWUniquePtr auth_ack_data(new chaos::common::data::CDataWrapper());
                            auth_ack_data->addInt32Value(AUTHORIZATION_STATE, 1);
                            sendRawMessage(ChaosMoveOperator(auth_ack_data));
                        }
                        };
                        }
                        }
                        }
                        }
                        
#endif /* __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h */

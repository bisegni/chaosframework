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

#include <chaos/common/external_unit/external_unit.h>
#include <chaos/common/external_unit/ExternalUnitServerEndpoint.h>

#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/thread/FutureHelper.h>
#include <chaos/common/utility/LockableObject.h>

#include <chaos/cu_toolkit/driver_manager/driver/AbstractDriverPlugin.h>

#include <boost/multi_index/member.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/hashed_index.hpp>

#define AUTHORIZATION_KEY           "authorization_key"
#define INIT_HARDWARE_PARAM         "ih_par"
#define AUTHORIZATION_STATE         "authorization_state"
#define CONFIGURATION_STATE         "configuration_state"
#define MESSAGE                     "msg"
#define MESSAGE_URI                 "uri"
#define MESSAGE_OPCODE              "opc"
#define MESSAGE_OPCODE_PARAMETER    "par"
#define REQUEST_IDENTIFICATION      "req_id"
#define REMOTE_LAYER_MESSAGE_TYPE   "msg_type"

#define AbstractRemoteIODriver_INFO    INFO_LOG(AbstractCDataWrapperIODriver)
#define AbstractRemoteIODriver_DBG     DBG_LOG(AbstractCDataWrapperIODriver)
#define AbstractRemoteIODriver_ERR     ERR_LOG(AbstractCDataWrapperIODriver)
#define REMOTE_DEFAULT_TIMEOUT 50000
namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //!liste of error
                typedef enum {
                    AR_ERROR_OK = 0,
                    AR_ERROR_NO_CONNECTION,
                    AR_ERROR_NOT_AUTORIZED,
                    AR_ERROR_NOT_CONFIGURED,
                    AR_ERROR_TIMEOUT,
                    AR_ERROR_LAST = 100
                } AR_ERROR;
                
                typedef chaos::common::thread::FutureHelper<chaos::common::data::CDWShrdPtr> CDWShrdPtrFutureHelper;
                
                template<typename EndpointType>
                class AbstractRemoteIODriver:
                public chaos::cu::driver_manager::driver::AbstractDriver,
                public chaos::cu::driver_manager::driver::AbstractDriverPlugin,
                public EndpointType {
                    // friend class AbstractServerRemoteIODriver;
                    friend class AbstractClientRemoteIODriver;
                protected:
                    typedef enum {
                        RDConnectionPhaseDisconnected,
                        RDConnectionPhaseConnected,
                        RDConnectionPhaseManageAutorization,//this fase is only osed by client implementation of driver
                        RDConnectionPhaseAutorized,
                        RDConnectionPhaseConfigured,
                    } RDConnectionPhase;
                private:
                    CHAOS_DEFINE_LOCKABLE_OBJECT(std::string, LString);
                    //!keep track of the current driver connection state
                    RDConnectionPhase   conn_phase;
                    //!the uri of the driver
                    std::string         remote_uri;
                    //!the uri instance to be used for api call subseguent the initialization phase
                    std::string         remote_uri_instance;
                    //!is the aturization key used to authenticate the remote parte
                    std::string         authorization_key;
                    //!is the indetifier of the remote conenction used by the sublayer
                    LString             current_connection_identifier;
                protected:
                    CDWShrdPtrFutureHelper future_hepler;
                    //message sent to remote endpoint when new connection has been received
                    chaos::common::data::CDWUniquePtr   driver_init_pack;
                    //!initialization and deinitialization driver methods
                    void driverInit(const char *initParameter) throw (chaos::CException) {
                        //AbstractRemoteIODriver_DBG <<" Initialization from string..."<<initParameter;
                    }
                    void driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
                        CHECK_ASSERTION_THROW_AND_LOG((init_parameter.isEmpty() == false), AbstractRemoteIODriver_ERR, -1, "Init parameter need to be formated in a json document");
                        //CHECK_ASSERTION_THROW_AND_LOG(init_parameter.hasKey(AUTHORIZATION_KEY), AbstractRemoteIODriver_ERR, -3, "The authorization key is mandatory")
                        //get the authorization key
                        AbstractRemoteIODriver_DBG <<" Initialization params:"<<init_parameter.getCompliantJSONString();
                        
                        if(init_parameter.hasKey(AUTHORIZATION_KEY)) {
                            authorization_key = init_parameter.getStringValue(AUTHORIZATION_KEY);
                        } else {
                            authorization_key = "";
                        }
                        
                        if(init_parameter.hasKey(INIT_HARDWARE_PARAM) &&
                           init_parameter.isCDataWrapperValue(INIT_HARDWARE_PARAM)) {
                            driver_init_pack=init_parameter.getCSDataValue(INIT_HARDWARE_PARAM);
                        }
                        future_hepler.init(NULL);
                    }
                    void driverDeinit() throw (chaos::CException) {
                        //send deinit, in case no one hase deinitlized before
                        _sendDeinitRequest();
                        //close future helper
                        CHAOS_NOT_THROW(future_hepler.deinit();)

                    }
                    bool checkAuthenticationState(chaos::common::data::CDWShrdPtr& message_response) {
                        bool result = false;
                        if(message_response->hasKey("err") &&
                           message_response->isInt32Value("err")) {
                            if(message_response->getInt32Value("err") != 0) {
                                return false;
                            }
                        }
                        if(message_response->hasKey(AUTHORIZATION_STATE) &&
                           message_response->isInt32Value(AUTHORIZATION_STATE)) {
                            result = (message_response->getInt32Value(AUTHORIZATION_STATE) == 1);
                        }
                        return result;
                    }
                    bool checkConfigurationState(chaos::common::data::CDWShrdPtr& message_response) {
                        bool result = true;
                        if(message_response->hasKey("err") &&
                           message_response->isInt32Value("err")) {
                            int err;
                            if((err=message_response->getInt32Value("err")) != 0) {
                                AbstractRemoteIODriver_ERR <<" Returned not zero err="<<err;
                                return false;
                            }
                        }
                        if(message_response->hasKey(CONFIGURATION_STATE) &&
                           message_response->isInt32Value(CONFIGURATION_STATE)) {
                            result = (message_response->getInt32Value(CONFIGURATION_STATE) == 1);
                        }
                        
                        if(message_response->hasKey(MESSAGE_URI) &&
                           message_response->isStringValue(MESSAGE_URI)) {
                            remote_uri_instance = message_response->getStringValue(MESSAGE_URI);
                        } else {
                            remote_uri_instance.clear();
                        }
                        return result;
                    }
                public:
                    AbstractRemoteIODriver():
                    chaos::cu::driver_manager::driver::AbstractDriverPlugin(this),
                    EndpointType(),
                    remote_uri(),
                    remote_uri_instance(),
                    authorization_key(),
                    conn_phase(RDConnectionPhaseDisconnected),
                    driver_init_pack(new chaos::common::data::CDataWrapper()){}
                    
                    ~AbstractRemoteIODriver(){}
                    
                    //!set a new uri for driver
                    void setDriverUri(std::string _new_remote_uri) {
                        remote_uri = _new_remote_uri;
                    }
                    
                    //!Send opcode request to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendOpcodeRequest(const std::string opcode,
                                          chaos::common::data::CDWUniquePtr message_data,
                                          chaos::common::data::CDWShrdPtr& message_response,
                                          uint32_t timeout = REMOTE_DEFAULT_TIMEOUT) {
                        int err = 0;
                        LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                        if((err = _managePhases())) {
                            return err;
                        }
                        return _sendRawOpcodeRequest(remote_uri_instance,
                                                     opcode,
                                                     MOVE(message_data),
                                                     message_response,
                                                     timeout);
                    }
                    
                    //!Send opcode message to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendOpcodeMessage(const std::string opcode,
                                          chaos::common::data::CDWUniquePtr message_data) {
                        int err = 0;
                        LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                        if((err = _managePhases())) {
                            return err;
                        }
                        return _sendRawOpcodeMessage(remote_uri_instance,
                                                     opcode,
                                                     MOVE(message_data));
                    }
                    
                    //!Send raw request to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = REMOTE_DEFAULT_TIMEOUT) {
                        int err = 0;
                        LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                        if((err = _managePhases())) {
                            return err;
                        }
                        return _sendRawRequest(MOVE(message_data),
                                               message_response,
                                               timeout);
                    }
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    int sendRawMessage(chaos::common::data::CDWUniquePtr message_data) {
                        int err = 0;
                        LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                        if((err = _managePhases())) {
                            return err;
                        }
                        return _sendRawMessage(MOVE(message_data));
                    }
                        
                    int sendInitRequest() {
                        int err = 0;
                        if((err = _sendAuthenticationRequest()) == 0) {
                            err = _sendInitRequest();
                        }
                        return err;
                    }
                    
                    int sendDeinitRequest() {
                        return _sendDeinitRequest();
                    }
                protected:
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
                    
                    const RDConnectionPhase getConnectionPhase() const {
                        return conn_phase;
                    }
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
                        int err = 0;
                        LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
                        if((err = _managePhases())) {
                            return err;
                        }
                        EndpointType::sendError(current_connection_identifier(),
                                                error_code, error_message, error_domain);
                        return AR_ERROR_OK;
                    }
                private:
                    
                    int handleReceivedeMessage(const std::string& connection_identifier,
                                               ChaosUniquePtr<chaos::common::data::CDataWrapper> message) {
                        if(message.get() == NULL) return 0;
                        //AbstractRemoteIODriver_DBG << CHAOS_FORMAT("Received message: %1%", %message->getCompliantJSONString());
                        if(message->hasKey(MESSAGE) == false) {
                            //error msg key is mandatory
                            //EndpointType::sendError(connection_identifier,
                            //                        -1, "message field is mandatory", __PRETTY_FUNCTION__);
                            AbstractRemoteIODriver_ERR<< CHAOS_FORMAT("[%1%]Message field is mandatory on connection", %connection_identifier);
                        } else if(message->isCDataWrapperValue(MESSAGE) == false) {
                            //error message need to be an object
                            //EndpointType::sendError(connection_identifier,
                            //                        -2, "message field need to be an object type", __PRETTY_FUNCTION__);
                            AbstractRemoteIODriver_ERR<< CHAOS_FORMAT("[%1%]Message field need to be an object type", %connection_identifier);
                        } else if(message->hasKey(REQUEST_IDENTIFICATION)) {
                            //we have a request
                            if(message->isInt32Value(REQUEST_IDENTIFICATION) == false) {
                                //error request id need to be a int32 value
                                //EndpointType::sendError(connection_identifier,
                                //                        -3, "request_id field need to be a int32 type", __PRETTY_FUNCTION__);
                                AbstractRemoteIODriver_ERR<< CHAOS_FORMAT("[%1%]request_id field need to be a int32 type", %connection_identifier);
                            }  else {
                                //we can forward
                                const uint32_t req_index = message->getUInt32Value(REQUEST_IDENTIFICATION);
                                ChaosUniquePtr<chaos::common::data::CDataWrapper> embedded_message(message->getCSDataValue(MESSAGE));
                                chaos::common::data::CDWShrdPtr message_shrd_ptr(embedded_message.release());
                                future_hepler.setDataForPromiseID(req_index, message_shrd_ptr);
                            }
                        } else if(message->hasKey(REMOTE_LAYER_MESSAGE_TYPE)){
                            if(message->isInt32Value(REMOTE_LAYER_MESSAGE_TYPE) == false) {
                                //error request id need to be a int32 value
                                //EndpointType::sendError(connection_identifier,
                                //                        -4, "msg_type field need to be a int32 type", __PRETTY_FUNCTION__);
                                AbstractRemoteIODriver_ERR<< CHAOS_FORMAT("[%1%]msg_type field need to be a int32 type", %connection_identifier);
                            }
                            asyncMessageReceived(MOVE(message));
                        } else {
                            //bad datapack received
                            //error request id need to be a int32 value
                            //EndpointType::sendError(connection_identifier,
                            //                        -5, "remote layer side message lack of msg_type key", __PRETTY_FUNCTION__);
                            AbstractRemoteIODriver_ERR<< CHAOS_FORMAT("[%1%]remote layer side message lack of msg_type key", %connection_identifier);
                        }
                        return 0;
                    }
                    
                    void sendAuthenticationACK() {
                        chaos::common::data::CDWUniquePtr auth_ack_data(new chaos::common::data::CDataWrapper());
                        auth_ack_data->addInt32Value(AUTHORIZATION_STATE, 1);
                        sendRawMessage(MOVE(auth_ack_data));
                    }
                    
                    //! manage the connection phases when a message or request need to be sent
                    int _managePhases() {
                        int err = 0;
                        switch(conn_phase) {
                            case RDConnectionPhaseDisconnected:
                                AbstractRemoteIODriver_DBG<<" RDConnectionPhaseDisconnected";
                                return AR_ERROR_NO_CONNECTION;
                                break;
                            case RDConnectionPhaseConnected: {
                                AbstractRemoteIODriver_DBG<<" Connected...";
                                CHAOS_ASSERT(current_connection_identifier().size());
                                if(authorization_key.size() != 0) {
                                    err = AR_ERROR_NOT_AUTORIZED;
                                    break;
                                } else {
                                    conn_phase = RDConnectionPhaseAutorized;
                                }
                            }
                                
                            case RDConnectionPhaseManageAutorization: {
//                                if(authorization_key.size() != 0) {
//                                    chaos::common::data::CDWShrdPtr message_response;
//                                    chaos::common::data::CDWUniquePtr auth_ack_data(new chaos::common::data::CDataWrapper());
//                                    AbstractRemoteIODriver_DBG<<" Connection OK, authorizing...";
//
//                                    auth_ack_data->addStringValue(AUTHORIZATION_KEY, authorization_key);
//                                    if((err = _sendRawOpcodeRequest(remote_uri,
//                                                                    "auth",
//                                                                    MOVE(auth_ack_data),
//                                                                    message_response)) ==0 ){
//                                        if(checkAuthenticationState(message_response)) {
//                                            conn_phase = RDConnectionPhaseAutorized;
//                                        } else {
//                                            AbstractRemoteIODriver_ERR<<" Authorization Fails, cannot configure";
//                                            err = AR_ERROR_NOT_AUTORIZED;
//                                            break;
//                                        }
//                                    }
//                                } else {
//                                    conn_phase = RDConnectionPhaseAutorized;
//                                }
                                if((err = _sendAuthenticationRequest()) != 0 ||
                                   conn_phase != RDConnectionPhaseAutorized ) {
                                    break;
                                }
                            }
                                
                            case RDConnectionPhaseAutorized: {
//                                chaos::common::data::CDWUniquePtr conf_msg(driver_init_pack->clone());
//                                chaos::common::data::CDWShrdPtr message_response;
//                                AbstractRemoteIODriver_DBG<<" Authorization OK, configuring...";
//                                if((err = _sendRawOpcodeRequest((remote_uri_instance.size()?remote_uri_instance:remote_uri),
//                                                                "init",
//                                                                MOVE(conf_msg),
//                                                                message_response)) ==0 ){
//                                    if(checkConfigurationState(message_response)) {
//                                        conn_phase = RDConnectionPhaseConfigured;
//                                    } else {
//                                        AbstractRemoteIODriver_ERR<<" Init Fails, Not Configured";
//                                        err = AR_ERROR_NOT_CONFIGURED;
//                                        break;
//                                    }
//                                }
                                ;
                                if((err = _sendInitRequest()) != 0 ||
                                   conn_phase != RDConnectionPhaseConfigured ) {
                                    break;
                                }
                            }
                                
                            case RDConnectionPhaseConfigured:
                                //we can proceeed
                                AbstractRemoteIODriver_DBG<<" Configuration OK!, start working";
                                break;
                        }
                        return err;
                    }
                    
                    int _sendAuthenticationRequest() {
                        int err = 0;
                        if(conn_phase == RDConnectionPhaseAutorized ||
                           conn_phase == RDConnectionPhaseConfigured) {
                            return 0;
                        }
                        if(conn_phase == RDConnectionPhaseManageAutorization) {
                            if(authorization_key.size() != 0) {
                                chaos::common::data::CDWShrdPtr message_response;
                                chaos::common::data::CDWUniquePtr auth_ack_data(new chaos::common::data::CDataWrapper());
                                AbstractRemoteIODriver_DBG<<"Sending authorizing request ...";
                                auth_ack_data->addStringValue(AUTHORIZATION_KEY, authorization_key);
                                if((err = _sendRawOpcodeRequest(remote_uri,
                                                                "auth",
                                                                MOVE(auth_ack_data),
                                                                message_response)) ==0 ){
                                    if(checkAuthenticationState(message_response)) {
                                        conn_phase = RDConnectionPhaseAutorized;
                                    } else {
                                        AbstractRemoteIODriver_ERR<<" Authorization Fails, cannot configure";
                                        err = AR_ERROR_NOT_AUTORIZED;
                                    }
                                }
                            } else {
                                conn_phase = RDConnectionPhaseAutorized;
                            }
                        } else {
                            err = -1;
                        }
                        return err;
                    }
                        
                    int _sendInitRequest() {
                        int err = 0;
                        if(conn_phase == RDConnectionPhaseConfigured) {
                            return 0;
                        }
                        if(conn_phase == RDConnectionPhaseAutorized) {
                            chaos::common::data::CDWUniquePtr conf_msg(driver_init_pack->clone());
                            chaos::common::data::CDWShrdPtr message_response;
                            AbstractRemoteIODriver_DBG<<" Authorization OK, configuring...";
                            if((err = _sendRawOpcodeRequest((remote_uri_instance.size()?remote_uri_instance:remote_uri),
                                                            "init",
                                                            MOVE(conf_msg),
                                                            message_response)) == 0 ){
                                if(checkConfigurationState(message_response)) {
                                    conn_phase = RDConnectionPhaseConfigured;
                                } else {
                                    AbstractRemoteIODriver_ERR<<" Init Fails, Not Configured";
                                    err = AR_ERROR_NOT_CONFIGURED;
                                }
                            }
                        } else {
                            err = -1;
                        }
                        return err;
                    }
                        
                    int _sendDeinitRequest() {
                        int err = 0;
                        if(conn_phase == RDConnectionPhaseAutorized ||
                           conn_phase == RDConnectionPhaseConfigured) {
                            chaos::common::data::CDWUniquePtr deinit_msg(new chaos::common::data::CDataWrapper());
                            chaos::common::data::CDWShrdPtr message_response;
                            if((err = _sendRawOpcodeRequest(remote_uri_instance,
                                                            "deinit",
                                                            MOVE(deinit_msg),
                                                            message_response)) == 0){
                                // AbstractRemoteIODriver_ERR << CHAOS_FORMAT("[%1%]Error deinitilizing remote driver on connection", %current_connection_identifie());
                                conn_phase = RDConnectionPhaseAutorized;
                                //reset instance uri
                                remote_uri_instance.clear();
                            }
                        } else {
                            return -1;
                        }
                        return err;
                    }
                    
                    //send datapack as opcode format
                    int _sendRawOpcodeRequest(const std::string uri,
                                              const std::string opcode,
                                              chaos::common::data::CDWUniquePtr message_data,
                                              chaos::common::data::CDWShrdPtr& message_response,
                                              uint32_t timeout = REMOTE_DEFAULT_TIMEOUT) {
                        chaos::common::data::CDWUniquePtr opcpde_msg(new chaos::common::data::CDataWrapper());
                        opcpde_msg->addStringValue(MESSAGE_URI, uri);
                        opcpde_msg->addStringValue(MESSAGE_OPCODE, opcode);
                        opcpde_msg->addCSDataValue(MESSAGE_OPCODE_PARAMETER, *message_data);
                        AbstractRemoteIODriver_DBG<<"sending "+uri+" opcode:"<<opcode<<" msg:"<<message_data->getCompliantJSONString();
                        return _sendRawRequest(MOVE(opcpde_msg),
                                               message_response,
                                               timeout);
                    }
                    int _sendRawOpcodeMessage(const std::string uri,
                                              const std::string opcode,
                                              chaos::common::data::CDWUniquePtr message_data) {
                        chaos::common::data::CDWUniquePtr opcpde_msg(new chaos::common::data::CDataWrapper());
                        opcpde_msg->addStringValue(MESSAGE_URI, uri);
                        opcpde_msg->addStringValue(MESSAGE_OPCODE, opcode);
                        opcpde_msg->addCSDataValue(MESSAGE_OPCODE_PARAMETER, *message_data);
                        return _sendRawMessage(MOVE(opcpde_msg));
                    }
                    int _sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                        chaos::common::data::CDWShrdPtr& message_response,
                                        uint32_t timeout = REMOTE_DEFAULT_TIMEOUT){
                        CDWShrdPtrFutureHelper::CounterType new_promise_id;
                        CDWShrdPtrFutureHelper::Future request_future;
                        chaos::common::data::CDWUniquePtr ext_msg(new chaos::common::data::CDataWrapper());
                        future_hepler.addNewPromise(new_promise_id,
                                                    request_future);
                        
                        ext_msg->addInt32Value(REQUEST_IDENTIFICATION, new_promise_id);
                        ext_msg->addCSDataValue(MESSAGE, *message_data);
                        
                        //send message to driver
                        EndpointType::sendMessage(current_connection_identifier(),
                                                  MOVE(ext_msg));
                        ChaosFutureStatus f_status = request_future.wait_for(ChaosCronoMilliseconds(timeout));
                        if(f_status == ChaosFutureStatus::ready) {
                            message_response = request_future.get();
                            return AR_ERROR_OK;
                        } else {
                            AbstractRemoteIODriver_ERR<<" Timeout Arised!";
                            return AR_ERROR_TIMEOUT;
                        }
                    }
                    int _sendRawMessage(chaos::common::data::CDWUniquePtr message_data){
                        //send message to driver
                        chaos::common::data::CDWUniquePtr ext_msg(new chaos::common::data::CDataWrapper());
                        ext_msg->addCSDataValue(MESSAGE, *message_data);
                        EndpointType::sendMessage(current_connection_identifier(),
                                                  MOVE(ext_msg));
                        //we have connection
                        return AR_ERROR_OK;
                    }
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__CE5788A_1D68_477E_BB9E_55BE8F7D3373_AbstractRemoteIODriver_h */



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

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>
#include <chaos/common/external_gateway/external_gateway.h>

#include <chaos/common/utility/TimingUtil.h>

#define INFO    INFO_LOG(AbstractCDataWrapperIODriver)
#define DBG		DBG_LOG(AbstractCDataWrapperIODriver)
#define ERR		ERR_LOG(AbstractCDataWrapperIODriver)

#define TIMEOUT_PURGE_PROMISE 60000000
#define PURGE_TIMER_REPEAT_DELAY 30000

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::async_central;

using namespace chaos::common::external_gateway;
using namespace chaos::cu::driver_manager::driver;

#define AUTHORIZATION_KEY        "authorization_key"
#define AUTHORIZATION_STATE      "authorization_state"
#define MESSAGE                 "message"
#define REQUEST_IDENTIFICATION  "request_id"

#pragma mark DriverResultInfo
const AbstractRemoteIODriver::DriverResultInfo::extract_index::result_type&
AbstractRemoteIODriver::DriverResultInfo::extract_index::operator()(const DriverResultInfo::DriverResultInfoShrdPtr &p) const {
    return p->request_id;
}

const AbstractRemoteIODriver::DriverResultInfo::extract_req_ts::result_type&
AbstractRemoteIODriver::DriverResultInfo::extract_req_ts::operator()(const DriverResultInfo::DriverResultInfoShrdPtr &p) const {
    return p->request_ts;
}

#pragma mark AbstractRemoteDriver
DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(AbstractRemoteIODriver),
ExternalUnitEndpoint(),
authorization_key(),
conn_phase(RDConnectionPhaseDisconnected),
message_counter(0),
set_p_req_id_index(boost::multi_index::get<tag_req_id>(set_p())),
set_p_req_ts_index(boost::multi_index::get<tag_req_ts>(set_p())){}

AbstractRemoteIODriver::~AbstractRemoteIODriver() {}

#pragma mark Public Method
void AbstractRemoteIODriver::driverInit(const char *initParameter) throw(chaos::CException) {
    INFO << "Init driver:"<<initParameter;
    CHECK_ASSERTION_THROW_AND_LOG(isDriverParamInJson(), ERR, -1, "Init parameter need to be formated in a json document");
    
    Json::Value root_param_document = getDriverParamJsonRootElement();
    
    Json::Value jv_endpoint_name = root_param_document["endpoint_name"];
    Json::Value jv_authorization_key = root_param_document[AUTHORIZATION_KEY];
    CHECK_ASSERTION_THROW_AND_LOG((jv_endpoint_name.isNull() == false), ERR, -2, "The endpoint name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG((jv_authorization_key.isNull() == false), ERR, -3, "The authorization key is mandatory");
    
    //! end point identifier & authorization key
    ExternalUnitEndpoint::endpoint_identifier = jv_endpoint_name.asString();
    authorization_key = jv_authorization_key.asString();
    CHECK_ASSERTION_THROW_AND_LOG(authorization_key.size(), ERR, -4, "The authorization key cannot be zero lenght");
    
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 PURGE_TIMER_REPEAT_DELAY,
                                                 PURGE_TIMER_REPEAT_DELAY);
    
    //register this driver as external endpoint
    ExternalUnitGateway::getInstance()->registerEndpoint(*this);
}

void AbstractRemoteIODriver::driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
    CHECK_ASSERTION_THROW_AND_LOG((init_parameter.isEmpty() == false), ERR, -1, "Init parameter need to be formated in a json document");
    
    CHECK_ASSERTION_THROW_AND_LOG(init_parameter.hasKey("endpoint_name"), ERR, -2, "The endpoint name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG(init_parameter.hasKey(AUTHORIZATION_KEY), ERR, -3, "The authorization key is mandatory");
    
    //! end point identifier & authorization key
    ExternalUnitEndpoint::endpoint_identifier = init_parameter.getStringValue("endpoint_name");
    authorization_key = init_parameter.getStringValue(AUTHORIZATION_KEY);
    CHECK_ASSERTION_THROW_AND_LOG(authorization_key.size(), ERR, -4, "The authorization key cannot be zero lenght");
    
    AsyncCentralManager::getInstance()->addTimer(this,
                                                 PURGE_TIMER_REPEAT_DELAY,
                                                 PURGE_TIMER_REPEAT_DELAY);
    
    //register this driver as external endpoint
    ExternalUnitGateway::getInstance()->registerEndpoint(*this);
}

void AbstractRemoteIODriver::driverDeinit() throw(chaos::CException) {
    INFO << "Deinit driver";
    //registerthis driver as external endpoint
    ExternalUnitGateway::getInstance()->deregisterEndpoint(*this);
    AsyncCentralManager::getInstance()->removeTimer(this);
}

void AbstractRemoteIODriver::handleNewConnection(const std::string& connection_identifier) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    current_connection_identifier() = connection_identifier;
    conn_phase = RDConnectionPhaseConnected;
}

void AbstractRemoteIODriver::handleDisconnection(const std::string& connection_identifier) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    current_connection_identifier().clear();
    conn_phase = RDConnectionPhaseDisconnected;
}

int AbstractRemoteIODriver::handleReceivedeMessage(const std::string& connection_identifier,
                                                   ChaosUniquePtr<CDataWrapper> message) {
    if(conn_phase != RDConnectionPhaseAutorized) {
        //check if a message with autorization key will arive
        if(message->hasKey(AUTHORIZATION_KEY) &&
           message->isStringValue(AUTHORIZATION_KEY)) {
            if(authorization_key.compare(message->getStringValue(AUTHORIZATION_KEY)) == 0){
                conn_phase = RDConnectionPhaseAutorized;
                sendAuthenticationACK();
            } else {
                //send error because not right type of req index
                ExternalUnitEndpoint::sendError(connection_identifier,
                                                -1, "Authentication failed", __PRETTY_FUNCTION__);
                ExternalUnitEndpoint::closeConnection(connection_identifier);
            }
        }
    } else {
        if(!message->hasKey(MESSAGE)){
            //send error because not right type of req index
            ExternalUnitEndpoint::sendError(connection_identifier,
                                            -2, "message field is mandatory", __PRETTY_FUNCTION__);
        } else if(!message->isCDataWrapperValue(MESSAGE)) {
            //send error because not right type of req index
            ExternalUnitEndpoint::sendError(connection_identifier,
                                            -3, "message field need to be an object type", __PRETTY_FUNCTION__);
        } else if(!message->hasKey(REQUEST_IDENTIFICATION)) {
            asyncMessageReceived(ChaosMoveOperator(message));
        } else if(!message->isInt32Value(REQUEST_IDENTIFICATION)) {
            //send error because not right type of req index
            ExternalUnitEndpoint::sendError(connection_identifier,
                                            -4, "request_id field need to be a int32 type", __PRETTY_FUNCTION__);
        }  else {
            //good request index
            const int64_t req_index = message->getUInt32Value(REQUEST_IDENTIFICATION);
            ChaosUniquePtr<CDataWrapper> embedded_message(message->getCSDataValue(MESSAGE));
            LSetPromiseWriteLock wl = set_p.getWriteLockObject();
            SetPromisesReqIdxIndexIter it = set_p_req_id_index.find(req_index);
            if(it != set_p_req_id_index.end()) {
                //set promises and remove it
                (*it)->promise.set_value(CDWShrdPtr(embedded_message.release()));
                set_p_req_id_index.erase(it);
            }
        }
    }
    return 0;
}

void AbstractRemoteIODriver::timeout() {
    LSetPromiseWriteLock wl = set_p.getWriteLockObject();
    uint64_t current_check_ts = TimingUtil::getTimeStampInMicroseconds();
    unsigned int max_purge_check = 10;
    for(SetPromisesReqTSIndexIter it = set_p_req_ts_index.begin(),
        end = set_p_req_ts_index.end();
        it != end && max_purge_check;
        max_purge_check--){
        //purge outdated promise
        if(current_check_ts > (*it)->request_ts+TIMEOUT_PURGE_PROMISE) {
            DBG << CHAOS_FORMAT("Remove the promise for request of index %1%", %(*it)->request_id);
            set_p_req_ts_index.erase(it++);
        } else {
            ++it;
        }
    }
}

void AbstractRemoteIODriver::sendAuthenticationACK() {
    CDWUniquePtr auth_ack_data(new CDataWrapper());
    auth_ack_data->addInt32Value(AUTHORIZATION_STATE, 1);
    sendRawMessage(ChaosMoveOperator(auth_ack_data));
}

int AbstractRemoteIODriver::sendRawRequest(CDWUniquePtr message_data,
                                           CDWShrdPtr& message_response,
                                           uint32_t timeout) {
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
    CDWUniquePtr ext_msg(new CDataWrapper());
    AbstractRemoteIODriver::DriverResultFuture request_future;
    DriverResultInfo::DriverResultInfoShrdPtr promise_info(new DriverResultInfo());
    promise_info->request_ts = TimingUtil::getTimeStampInMicroseconds();
    promise_info->request_id = message_counter++;
    
    ext_msg->addInt32Value(REQUEST_IDENTIFICATION, promise_info->request_id);
    ext_msg->addCSDataValue(MESSAGE, *message_data);
    //store promises in result map
    LSetPromiseWriteLock lmr_wl = set_p.getWriteLockObject();
    set_p().insert(promise_info);
    lmr_wl->unlock();
    //send message to driver
    ExternalUnitEndpoint::sendMessage(current_connection_identifier(),
                                      ChaosMoveOperator(ext_msg));
    //set the
    request_future = promise_info->promise.get_future();
    ChaosFutureStatus f_status = request_future.wait_for(ChaosCronoMilliseconds(timeout));
    if(f_status == ChaosFutureStatus::ready) {
        message_response = request_future.get();
        return AR_ERROR_OK;
    } else {
        return AR_ERROR_TIMEOUT;
    }
}

int AbstractRemoteIODriver::sendRawMessage(CDWUniquePtr message_data) {
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
    ExternalUnitEndpoint::sendMessage(current_connection_identifier(),
                                      ChaosMoveOperator(message_data));
    //we have connection
    return AR_ERROR_OK;
}
int AbstractRemoteIODriver::sendError(int error_code,
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
    ExternalUnitEndpoint::sendError(current_connection_identifier(),
                                    error_code, error_message, error_domain);
    return AR_ERROR_OK;
}

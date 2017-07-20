/*
 *	AbstractRemoteIODriver.cpp
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

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>
#include <chaos/cu_toolkit/external_gateway/external_gateway.h>

#include <chaos/common/utility/TimingUtil.h>

#define INFO    INFO_LOG(AbstractCDataWrapperIODriver)
#define DBG		DBG_LOG(AbstractCDataWrapperIODriver)
#define ERR		ERR_LOG(AbstractCDataWrapperIODriver)

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::cu::external_gateway;
using namespace chaos::cu::driver_manager::driver;

DEFAULT_CU_DRIVER_PLUGIN_CONSTRUCTOR(AbstractRemoteIODriver),
ExternalUnitEndpoint(),
message_counter(0),
authorization_key(){}

AbstractRemoteIODriver::~AbstractRemoteIODriver() {}

#pragma mark Public Method
void AbstractRemoteIODriver::driverInit(const char *initParameter) throw(chaos::CException) {
    INFO << "Init driver:"<<initParameter;
    CHECK_ASSERTION_THROW_AND_LOG(isDriverParamInJson(), ERR, -1, "Init parameter need to be formated in a json document");
    
    Json::Value root_param_document = getDriverParamJsonRootElement();
    
    Json::Value jv_endpoint_name = root_param_document["endpoint_name"];
    Json::Value jv_autorization_key = root_param_document["authorization_key"];
    CHECK_ASSERTION_THROW_AND_LOG((jv_endpoint_name.isNull() == false), ERR, -2, "The endpoint name is mandatory");
    CHECK_ASSERTION_THROW_AND_LOG((jv_autorization_key.isNull() == false), ERR, -3, "The authorization key is mandatory");
    
    //! end point identifier & authorization key
    ExternalUnitEndpoint::endpoint_identifier = jv_endpoint_name.asString();
    authorization_key = jv_autorization_key.asString();
    //register this driver as external endpoint
    ExternalUnitGateway::getInstance()->registerEndpoint(*this);
}

void AbstractRemoteIODriver::driverDeinit() throw(chaos::CException) {
    INFO << "Deinit driver";
    //registerthis driver as external endpoint
    ExternalUnitGateway::getInstance()->deregisterEndpoint(*this);
}

void AbstractRemoteIODriver::handleNewConnection(const std::string& connection_identifier) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    current_connection_identifier() = connection_identifier;
}

void AbstractRemoteIODriver::handleDisconnection(const std::string& connection_identifier) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    current_connection_identifier().clear();
}

int AbstractRemoteIODriver::handleReceivedeMessage(const std::string& connection_identifier,
                                                   ChaosUniquePtr<CDataWrapper> message) {
    return 0;
}

void AbstractRemoteIODriver::composeRequest(chaos::common::data::CDWUniquePtr& ext_msg,
                                            uint64_t request_index,
                                            MessageType message_type,
                                            chaos::common::data::CDWUniquePtr& message_data) {
    CHAOS_ASSERT(ext_msg)
    ext_msg->addInt32Value("type", (int32_t)message_type);
    composeMessage(ext_msg,
                   message_type,
                   message_data);
}

void AbstractRemoteIODriver::composeMessage(chaos::common::data::CDWUniquePtr& ext_msg,
                                            MessageType message_type,
                                            chaos::common::data::CDWUniquePtr& message_data) {
    CHAOS_ASSERT(ext_msg)
    ext_msg->addInt32Value("reqeust_index", (int32_t)message_type);
    ext_msg->addCSDataValue("message", *message_data);
}

void AbstractRemoteIODriver::timeout() {
    
}
#pragma mark Public API
int AbstractRemoteIODriver::getDriverDataset(CDWUniquePtr& received_data) {
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::readVariable(const std::string& variable,
                                         const CDataVariant& value) {
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::writeVariable(const std::string& variable,
                                          const CDataVariant& value) {
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::sendRawRequest(MessageType message_type,
                                           CDWUniquePtr message_data,
                                           AbstractRemoteIODriver::DriverResultFuture& request_future) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    if(current_connection_identifier().size() == 0) return AR_ERROR_NO_CONNECTION;
    
    DriverResultInfoShrdPtr promise_info(new DriverResultInfo());
    promise_info->request_ts = TimingUtil::getTimeStamp();
    
    uint64_t current_index = message_counter++;
    CDWUniquePtr ext_message(new CDataWrapper());
    composeRequest(ext_message,
                   current_index,
                   message_type,
                   message_data);
    
    //store promises in result map
    LMapResultWriteLock lmr_wl = map_of_promises.getWriteLockObject();
    map_of_promises().insert(MapResultPair(current_index,
                                           promise_info));
    
    //send message to driver
    ExternalUnitEndpoint::sendMessage(current_connection_identifier(),
                                      ChaosMoveOperator(ext_message));
    //set the
    request_future = promise_info->promise.get_future();
    return AR_ERROR_OK;
}

int AbstractRemoteIODriver::sendRawMessage(MessageType message_type,
                                           CDWUniquePtr message_data) {
    LStringWriteLock wl = current_connection_identifier.getWriteLockObject();
    if(current_connection_identifier().size() == 0) return AR_ERROR_NO_CONNECTION;
    
    CDWUniquePtr ext_message(new CDataWrapper());
    composeMessage(ext_message,
                   message_type,
                   message_data);
    
    //send message to driver
    ExternalUnitEndpoint::sendMessage(current_connection_identifier(),
                                      ChaosMoveOperator(ext_message));
    //we have connection
    return AR_ERROR_OK;
}

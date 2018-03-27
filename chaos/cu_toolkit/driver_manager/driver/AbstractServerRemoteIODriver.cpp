/*
 * Copyright 2012, 18/10/2017 INFN
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
#include <chaos/cu_toolkit/driver_manager/driver/AbstractServerRemoteIODriver.h>

using namespace chaos::common::data;
using namespace chaos::common::async_central;
using namespace chaos::cu::driver_manager::driver;

#define INFO    INFO_LOG(AbstractServerRemoteIODriver)
#define DBG     DBG_LOG(AbstractServerRemoteIODriver)
#define ERR     ERR_LOG(AbstractServerRemoteIODriver)

void AbstractServerRemoteIODriver::driverInit(const char *initParameter) throw (chaos::CException) {
    INFO << "Init driver:"<<initParameter;
//    CHECK_ASSERTION_THROW_AND_LOG(isDriverParamInJson(), ERR, -1, "Init parameter need to be formated in a json document");
//
//    Json::Value root_param_document = getDriverParamJsonRootElement();
//
//    Json::Value jv_endpoint_name = root_param_document["endpoint_name"];
//    CHECK_ASSERTION_THROW_AND_LOG((jv_endpoint_name.isNull() == false), ERR, -2, "The endpoint name is mandatory");
//
//    //! end point identifier & authorization key
//    ExternalUnitServerEndpoint::endpoint_identifier = jv_endpoint_name.asString();
//    CHECK_ASSERTION_THROW_AND_LOG((ExternalUnitServerEndpoint::endpoint_identifier.size() > 0), ERR, -4, "The endpoint name is empty");
//
//    //check if a driver uri has been set
//    if(init_parameter.hasKey("uri") &&
//       init_parameter.isStringValue("uri")) {
//        setDriverUri(init_parameter.getStringValue("uri"));
//    }
//
//    ServerARIODriver::driverInit(initParameter);
//
//    //register this driver as external endpoint
//    chaos::common::external_unit::ExternalUnitManager::getInstance()->registerEndpoint(*this);
    CHAOS_ASSERT(true);
}
void AbstractServerRemoteIODriver::driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
    CHECK_ASSERTION_THROW_AND_LOG((init_parameter.isEmpty() == false), ERR, -1, "Init parameter need to be formated in a json document");
    CHECK_ASSERTION_THROW_AND_LOG(init_parameter.hasKey("endpoint_name"), ERR, -2, "The endpoint name is mandatory");
    
    //! end point identifier & authorization key
    ExternalUnitServerEndpoint::endpoint_identifier = init_parameter.getStringValue("endpoint_name");
    CHECK_ASSERTION_THROW_AND_LOG((ExternalUnitServerEndpoint::endpoint_identifier.size() > 0), ERR, -4, "The endpoint name is empty");
    
    //check if a driver uri has been set
    if(init_parameter.hasKey("uri") &&
       init_parameter.isStringValue("uri")) {
        setDriverUri(init_parameter.getStringValue("uri"));
    }
    
    ServerARIODriver::driverInit(init_parameter);
    
    //register this driver as external endpoint
    chaos::common::external_unit::ExternalUnitManager::getInstance()->registerEndpoint(*this);
}

void AbstractServerRemoteIODriver::driverDeinit() throw (chaos::CException) {
    INFO << "Deinit driver";
    //registerthis driver as external endpoint
    chaos::common::external_unit::ExternalUnitManager::getInstance()->deregisterEndpoint(*this);
    CHAOS_NOT_THROW(ServerARIODriver::driverDeinit();)
}

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
#include <chaos/cu_toolkit/driver_manager/driver/AbstractClientRemoteIODriver.h>

#define INFO    INFO_LOG(AbstractClientRemoteIODriver)
#define DBG     DBG_LOG(AbstractClientRemoteIODriver)
#define ERR     ERR_LOG(AbstractClientRemoteIODriver)

using namespace chaos::cu::driver_manager::driver;

void AbstractClientRemoteIODriver::driverInit(const char *initParameter) throw (chaos::CException) {
    LOG_AND_TROW(ERR, -1, "AbstractClientRemoteIODriver can be initilized only with json document");
}
void AbstractClientRemoteIODriver::driverInit(const chaos::common::data::CDataWrapper& init_parameter) throw(chaos::CException) {
    CHECK_ASSERTION_THROW_AND_LOG((init_parameter.isEmpty() == false), ERR, -1, "Init parameter need to be formated in a json document");
    CHECK_ASSERTION_THROW_AND_LOG(init_parameter.hasKey("uri"), ERR, -2, "The hostname name is mandatory");
    const std::string uri = init_parameter.getStringValue("uri");
    CHECK_ASSERTION_THROW_AND_LOG(uri.size() != 0, ERR, -3, "The uri parameter can't be empty string");
    //! end point identifier & authorization key
    if(init_parameter.hasKey("endpoint_name")){
        ExternalUnitClientEndpoint::endpoint_identifier = init_parameter.getStringValue("endpoint_name");
    } else {
        ExternalUnitClientEndpoint::endpoint_identifier = init_parameter.getStringValue("uri");
    }
    CHECK_ASSERTION_THROW_AND_LOG((ExternalUnitClientEndpoint::endpoint_identifier.size() > 0), ERR, -4, "The endpoint name is empty");
    //register this driver as external endpoint
    chaos::common::external_unit::ExternalUnitManager::getInstance()->initilizeConnection(*this,
                                                                                          "http",
                                                                                          "application/bson-json",
                                                                                          uri);
    
    
    ClientARIODriver::driverInit(init_parameter);
}
void AbstractClientRemoteIODriver::driverDeinit() throw (chaos::CException) {
    INFO << "Deinit driver";
    chaos::common::external_unit::ExternalUnitManager::getInstance()->releaseConnection(*this,
                                                                                        "http");
    CHAOS_NOT_THROW(ClientARIODriver::driverDeinit();)
}

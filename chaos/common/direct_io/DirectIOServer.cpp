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
#include <chaos/common/direct_io/DirectIOServer.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/format.hpp>

using namespace chaos::common::utility;
using namespace chaos::common::direct_io;

DirectIOServer::DirectIOServer(const std::string& alias):
NamedService(alias),
priority_port(0),
service_port(0),
handler_impl(NULL){
}

DirectIOServer::~DirectIOServer() {
    clearHandler();
}

// Initialize instance
void DirectIOServer::init(void *init_data)  {
    if(handler_impl == NULL) throw chaos::CException(-1, "handler has not been configured", __FUNCTION__);
    StartableService::initImplementation(handler_impl, init_data, "DirectIOServer handler", __FUNCTION__);
    //construct the direct io service url
    service_url = boost::str( boost::format("%1%:%2%:%3%") % chaos::GlobalConfiguration::getInstance()->getLocalServerAddress() % priority_port % service_port);
}

// Start the implementation
void DirectIOServer::start()  {
    StartableService::startImplementation(handler_impl, "DirectIOServer handler", __FUNCTION__);
}

// Stop the implementation
void DirectIOServer::stop()  {
    StartableService::stopImplementation(handler_impl, "DirectIOServer handler", __FUNCTION__);
}

// Deinit the implementation
void DirectIOServer::deinit()  {
    StartableService::deinitImplementation(handler_impl, "DirectIOServer handler", __FUNCTION__);
}

//! Send some data to the server
void DirectIOServer::setHandler(DirectIODispatcher *_handler_impl) {
    if(StartableService::getServiceState() != CUStateKey::DEINIT)
        return;
    handler_impl = _handler_impl;
    handler_impl->server_public_interface = this;
}

//! Remove the handler pointer
void DirectIOServer::clearHandler() {
    if(StartableService::getServiceState() != CUStateKey::DEINIT)
        return;
    if(handler_impl) delete(handler_impl);
}
uint32_t DirectIOServer::getPriorityPort() {
    return priority_port;
}

uint32_t DirectIOServer::getServicePort() {
    return service_port;
}

const std::string& DirectIOServer::getUrl() {
    return  service_url;
}

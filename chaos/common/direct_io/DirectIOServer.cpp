/*
 *	DirectIOServer.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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
void DirectIOServer::init(void *init_data) throw(chaos::CException) {
    if(handler_impl == NULL) throw chaos::CException(-1, "handler has not been configured", __FUNCTION__);
    StartableService::initImplementation(handler_impl, init_data, "DirectIOServer handler", __FUNCTION__);
    //construct the direct io service url
    service_url = boost::str( boost::format("%1%:%2%:%3%") % chaos::GlobalConfiguration::getInstance()->getLocalServerAddress() % priority_port % service_port);
}

// Start the implementation
void DirectIOServer::start() throw(chaos::CException) {
    StartableService::startImplementation(handler_impl, "DirectIOServer handler", __FUNCTION__);
}

// Stop the implementation
void DirectIOServer::stop() throw(chaos::CException) {
    StartableService::stopImplementation(handler_impl, "DirectIOServer handler", __FUNCTION__);
}

// Deinit the implementation
void DirectIOServer::deinit() throw(chaos::CException) {
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

void DirectIOServer::deleteDataWithHandler(DirectIODeallocationHandler *_data_deallocator,
                                           DisposeSentMemoryInfo::SentPart _sent_part,
                                           uint16_t _sent_opcode,
                                           void *data) {
    if(_data_deallocator && data) {
        CLEAN_DIO_DATA_WITH_HANDLER(_data_deallocator,
                                    _sent_part,
                                    _sent_opcode,
                                    data);
        data = NULL;
    }
}
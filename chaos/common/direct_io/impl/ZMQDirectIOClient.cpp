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
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOClient.h>
#include <chaos/common/data/cache/FastHash.h>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include <string.h>
#include <assert.h>     /* assert */

#define ZMQDIO_LOG_HEAD "["<<getName()<<"] - "

#define ZMQDIOLAPP_ LAPP_ << ZMQDIO_LOG_HEAD
#define ZMQDIOLDBG_ LDBG_ << ZMQDIO_LOG_HEAD << __FUNCTION__ << " - "
#define ZMQDIOLERR_ LERR_ << ZMQDIO_LOG_HEAD

using namespace chaos::common::utility;

using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::impl;

typedef boost::unique_lock<boost::shared_mutex>	ZMQDirectIOClientWriteLock;
typedef boost::shared_lock<boost::shared_mutex> ZMQDirectIOClientReadLock;

DEFINE_CLASS_FACTORY(ZMQDirectIOClient, DirectIOClient);

//------------------------------STATIC METHOD---------------------------------


ZMQDirectIOClient::ZMQDirectIOClient(std::string alias):
DirectIOClient(alias),
priority_port(0),
service_port(0),
thread_run(false),
zmq_context(NULL){};

ZMQDirectIOClient::~ZMQDirectIOClient(){};

//! Initialize instance
void ZMQDirectIOClient::init(void *init_data) throw(chaos::CException) {
    int err = 0;
    MapZMQConfiguration default_configuration;
    default_configuration["ZMQ_IO_THREADS"] = "1";
    
    DirectIOClient::init(init_data);
    ZMQDIOLAPP_ << "Allocating zmq context";
    thread_run= true;
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __FUNCTION__);
    if((err = ZMQBaseClass::configureContextWithStartupParameter(zmq_context,
                                                                 default_configuration,
                                                                 chaos::GlobalConfiguration::getInstance()->getDirectIOClientImplKVParam(),
                                                                 "ZMQ DirectIO Client"))) {
        throw chaos::CException(2, "Error configuring service socket", __FUNCTION__);
    }
    
    
    ZMQDIOLAPP_ << "Inizilizing zmq implementation with zmq lib version = " << ZMQ_VERSION;
    ZMQDIOLAPP_ << "Set number of thread for the contex";
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    
    ZMQDIOLAPP_ << "Initialized";
}

//! Deinit the implementation
void ZMQDirectIOClient::deinit() throw(chaos::CException) {
    int err = 0;
    //remove all active connection (never need to be exists at this step)
    map_connections.clearElement();
    //destroy the zmq context
    ZMQDIOLAPP_ << "Destroing zmq context";
    thread_run = false;
    err = zmq_ctx_destroy(zmq_context);
    if(err) ZMQDIOLERR_ << "Error closing context";
    //monitor_thread_group.join_all();
    
    zmq_context = NULL;
    ZMQDIOLAPP_ << "ZMQ context destroyed";
    DirectIOClient::deinit();
}

DirectIOClientConnection *ZMQDirectIOClient::_getNewConnectionImpl(std::string server_description,
                                                                   uint16_t endpoint) {
    //allocate client
    ZMQDirectIOClientConnection *connection = new ZMQDirectIOClientConnection(zmq_context,
                                                                              server_description,
                                                                              endpoint);
    if(connection == NULL) return NULL;
    try{
        InizializableService::initImplementation(connection, NULL, "ZMQDirectIOClientConnection", __PRETTY_FUNCTION__);
        //register client with the hash of the xzmq decoded endpoint address (tcp://ip:port)
        DEBUG_CODE(ZMQDIOLDBG_ << "Register client for " << server_description << " with zmq decoded hash " << connection->getUniqueUUID();)
        map_connections.registerElement(connection->getUniqueUUID(), connection);
    } catch (...) {
        ZMQDIOLERR_ << CHAOS_FORMAT("We got error initilizing connection to %1%:%2% so we goning to deinitilize it an return NULL channel", %server_description%endpoint);
        //in case of error
        CHAOS_NOT_THROW(InizializableService::deinitImplementation(connection, "ZMQDirectIOClientConnection", __PRETTY_FUNCTION__););
        connection = NULL;
    }
    return connection;
}

void ZMQDirectIOClient::_releaseConnectionImpl(DirectIOClientConnection *connection_to_release) {
    ZMQDirectIOClientConnection *conn=reinterpret_cast<ZMQDirectIOClientConnection*>(connection_to_release);
    if(!conn) return;
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(conn, "ZMQDirectIOClientConnection", __PRETTY_FUNCTION__););
    //CHAOS_ASSERT(conn->monitor_info)
    //stop the monitor
    DEBUG_CODE(ZMQDIOLDBG_ << "Release the connection for: " << connection_to_release->getServerDescription() <<" ptr:"<<std::hex<<(uint64_t)connection_to_release;)
    map_connections.deregisterElementKey(conn->getUniqueUUID());
    delete(connection_to_release);
}

void ZMQDirectIOClient::freeObject(const DCKeyObjectContainer::TKOCElement& element) {
    if(!element.element) return;
    DirectIOClientConnection *connection = element.element;
    DEBUG_CODE(ZMQDIOLDBG_ << "Autorelease connection for " << connection->getServerDescription();)
    releaseConnection(connection);
}

/*
 *	ZMQDirectIOServer.cpp
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

#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/utility/UUIDUtil.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/DirectIODataPack.h>
#include <chaos/common/direct_io/impl/ZMQDirectIOServer.h>
#include <boost/format.hpp>


#define ZMQDIO_SRV_LOG_HEAD "["<<getName()<<"] - "

#define ZMQDIO_SRV_LAPP_ LAPP_ << ZMQDIO_SRV_LOG_HEAD
#define ZMQDIO_SRV_LDBG_ LDBG_ << ZMQDIO_SRV_LOG_HEAD
#define ZMQDIO_SRV_LERR_ LERR_ << ZMQDIO_SRV_LOG_HEAD


#define DIRECTIO_FREE_ANSWER_DATA(x)\
if(x && x->answer_data) free(x->answer_data);\
if(x) free(x);\
x = NULL;

#define INPROC_PRIORITY "inproc://priority"
#define INPROC_SERVICE "inproc://service"

namespace chaos_data = chaos::common::data;

using namespace chaos::common::direct_io::impl;
using namespace chaos::common::direct_io;

DEFINE_CLASS_FACTORY(ZMQDirectIOServer, DirectIOServer);

ZMQDirectIOServer::ZMQDirectIOServer(std::string alias):
DirectIOServer(alias),
zmq_context(NULL),
priority_socket(NULL),
service_socket(NULL),
run_server(NULL){};

ZMQDirectIOServer::~ZMQDirectIOServer(){};

//! Initialize instance
void ZMQDirectIOServer::init(void *init_data) throw(chaos::CException) {
    
    chaos_data::CDataWrapper *init_cw = static_cast<chaos_data::CDataWrapper*>(init_data);
    if(!init_cw) throw chaos::CException(0, "No configration has been provided", __PRETTY_FUNCTION__);
    
    //get the port from configuration
    priority_port = init_cw->getInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_PRIORITY_PORT);
    if(priority_port <= 0) throw chaos::CException(0, "Bad priority port configured", __PRETTY_FUNCTION__);
    
    service_port = init_cw->getInt32Value(common::direct_io::DirectIOConfigurationKey::DIRECT_IO_SERVICE_PORT);
    if(service_port <= 0) throw chaos::CException(0, "Bad service port configured", __PRETTY_FUNCTION__);
    DirectIOServer::init(init_data);
    
    //create the endpoint strings
    priority_socket_bind_str = boost::str( boost::format("tcp://*:%1%") % priority_port);
    ZMQDIO_SRV_LAPP_ << "priority socket bind url: " << priority_socket_bind_str;
    
    service_socket_bind_str = boost::str( boost::format("tcp://*:%1%") % service_port);
    ZMQDIO_SRV_LAPP_ << "service socket bind url: " << service_socket_bind_str;
}

//! Start the implementation
void ZMQDirectIOServer::start() throw(chaos::CException) {
    int err = 0;
    MapZMQConfiguration         default_context_configuration;
    default_context_configuration["ZMQ_IO_THREADS"] = "1";
    
    int direct_io_thread_number = 2;
    DirectIOServer::start();
    run_server = true;
    
    //get custm configuration for direct io server
    if(GlobalConfiguration::getInstance()->hasOption(InitOption::OPT_DIRECT_IO_SERVER_THREAD_NUMBER)) {
        direct_io_thread_number = GlobalConfiguration::getInstance()->getOption<int>(InitOption::OPT_DIRECT_IO_SERVER_THREAD_NUMBER);
    }
    
    //create the ZMQContext
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __PRETTY_FUNCTION__);
    if((err = ZMQBaseClass::configureContextWithStartupParameter(zmq_context,
                                                                 default_context_configuration,
                                                                 chaos::GlobalConfiguration::getInstance()->getDirectIOServerImplKVParam(),
                                                                 "ZMQ DirectIO Server"))) {
        throw chaos::CException(err, "Error configuring zmq context", __PRETTY_FUNCTION__);
    }
    
    //queue thread
    ZMQDIO_SRV_LAPP_ << "Allocating and binding priority socket to "<< priority_socket_bind_str;
    ZMQDIO_SRV_LAPP_ << "Allocating threads for manage the requests";
    try{
        //start the treads for the proxies
        server_threads_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOServer::poolerPriority,
                                                                      this)));
        server_threads_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOServer::poolerService,
                                                                      this)));
        //thread for service worker
        server_threads_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOServer::worker,
                                                                      this,
                                                                      INPROC_SERVICE,
                                                                      &DirectIOHandler::serviceDataReceived)));
        
        //threads for priority worker
        for(int idx_thrd = 0;
            idx_thrd < direct_io_thread_number;
            idx_thrd++) {
            server_threads_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOServer::worker,
                                                                          this,
                                                                          INPROC_PRIORITY,
                                                                          &DirectIOHandler::priorityDataReceived)));
        }
        ZMQDIO_SRV_LAPP_ << CHAOS_FORMAT("ZMQ high priority socket managed by %1% threads", %direct_io_thread_number);
    } catch(boost::exception_detail::clone_impl<boost::exception_detail::error_info_injector<boost::lock_error> >& lock_error_exception) {
        ZMQDIO_SRV_LERR_ << lock_error_exception.what();
        throw chaos::CException(0, std::string(lock_error_exception.what()), __PRETTY_FUNCTION__);
    }
    ZMQDIO_SRV_LAPP_ << "Threads allocated and started";
}

//! Stop the implementation
void ZMQDirectIOServer::stop() throw(chaos::CException) {
    run_server = false;
    ZMQDIO_SRV_LAPP_ << "Deallocating zmq context";
    zmq_ctx_shutdown(zmq_context);
    zmq_ctx_term(zmq_context);
    ZMQDIO_SRV_LAPP_ << "ZMQ Context deallocated";
    
    //wiath all thread
    ZMQDIO_SRV_LAPP_ << "Join on all thread";
    server_threads_group.join_all();
    ZMQDIO_SRV_LAPP_ << "All thread stopped";
    DirectIOServer::stop();
}

//! Deinit the implementation
void ZMQDirectIOServer::deinit() throw(chaos::CException) {
    //serverThreadGroup.stopGroup(true);
    
    DirectIOServer::deinit();
}

void ZMQDirectIOServer::poolerPriority() {
    int err = 0;
    
    MapZMQConfiguration         default_socket_configuration;
    default_socket_configuration["ZMQ_LINGER"] = "500";
    default_socket_configuration["ZMQ_RCVHWM"] = "1000";
    default_socket_configuration["ZMQ_SNDHWM"] = "1000";
    default_socket_configuration["ZMQ_RCVTIMEO"] = "-1";
    default_socket_configuration["ZMQ_SNDTIMEO"] = "1000";
    ZMQDIO_SRV_LAPP_ << "Enter poolerPriority";
    //start creating two socker for service and priority
    ZMQDIO_SRV_LAPP_ << "Allocating and binding priority socket to "<< priority_socket_bind_str;
    priority_socket = zmq_socket (zmq_context, ZMQ_ROUTER);
    if(priority_socket == NULL){
        return;
    }
    if((err = zmq_bind(priority_socket, priority_socket_bind_str.c_str()))){
        return;
    }
    if((err = ZMQBaseClass::configureSocketWithStartupParameter(priority_socket,
                                                                default_socket_configuration,
                                                                chaos::GlobalConfiguration::getInstance()->getDirectIOServerImplKVParam(),
                                                                "ZMQ DirectIO Server Priority"))){
        return;
    }
    //create proxy for priority
    priority_proxy = zmq_socket (zmq_context, ZMQ_DEALER);
    if(priority_proxy == NULL) {
        return;
    }
    if((err = zmq_bind(priority_proxy, INPROC_PRIORITY))) {
        return;
    }

    try {
        zmq_proxy(priority_socket, priority_proxy, NULL);
    }catch (std::exception &e) {}
    if(priority_socket) {
        if((err = zmq_unbind(priority_socket, priority_socket_bind_str.c_str()))){
            ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% unbindind priority socket", %err);
        }
        if((err = zmq_close(priority_socket))){
            ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% closing priority socket", %err);
        }
        if(priority_proxy) {
            if((err = zmq_close(priority_proxy))){
                ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% closing priority proxy", %err);
            }
            priority_proxy = NULL;
        }
        priority_socket = NULL;
    }
    ZMQDIO_SRV_LAPP_ << "Leaving poolerPriority";
}
void ZMQDirectIOServer::poolerService() {
    int err = 0;
    
    MapZMQConfiguration         default_socket_configuration;
    default_socket_configuration["ZMQ_LINGER"] = "500";
    default_socket_configuration["ZMQ_RCVHWM"] = "1000";
    default_socket_configuration["ZMQ_SNDHWM"] = "1000";
    default_socket_configuration["ZMQ_RCVTIMEO"] = "-1";
    default_socket_configuration["ZMQ_SNDTIMEO"] = "1000";
    ZMQDIO_SRV_LAPP_ << "Enter poolerPriority";
    //allocate service socket
    ZMQDIO_SRV_LAPP_ << "Allocating and binding service socket to "<< priority_socket_bind_str;
    service_socket = zmq_socket (zmq_context, ZMQ_ROUTER);
    if(service_socket == NULL){
        return;
    }
    if((err = zmq_bind(service_socket, service_socket_bind_str.c_str()))){
        return;
    }
    if((err = ZMQBaseClass::configureSocketWithStartupParameter(service_socket,
                                                                default_socket_configuration,
                                                                chaos::GlobalConfiguration::getInstance()->getDirectIOServerImplKVParam(),
                                                                "ZMQ DirectIO Server Service"))){
        return;
    }
    //create proxy for priority
    service_proxy = zmq_socket (zmq_context, ZMQ_DEALER);
    if(service_proxy == NULL) {
        return;
    }
    if((err = zmq_bind(service_proxy, INPROC_SERVICE))){
        return;
    }
    try {
        zmq_proxy(service_socket, service_proxy, NULL);
    }catch (std::exception &e) {}
    if(service_socket) {
        if((err = zmq_unbind(service_socket, service_socket_bind_str.c_str()))){
            ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% unbindind service socket", %err);
        }
        if((err = zmq_close(service_socket))){
            ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% closing service socket", %err);
        }
        if(service_socket) {
            if((err = zmq_close(service_socket))){
                ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% closing service proxy", %err);
            }
            priority_proxy = NULL;
        }
        service_socket = NULL;
    }

    ZMQDIO_SRV_LAPP_ << "Leaving poolerPriority";
}
void ZMQDirectIOServer::worker(const std::string& bind_inproc_url,
                               DirectIOHandlerPtr delegate) {
    int err = 0;
    std::string                 identity;
    void						*worker_socket          = NULL;
    bool						send_synchronous_answer = false;
    DirectIODataPack			*data_pack			= NULL;
    DirectIODataPack            *data_pack_answer   = NULL;
    DirectIODataPack            data_pack_answer_stack_alloc;
    DirectIODeallocationHandler *answer_header_deallocation_handler = NULL;
    DirectIODeallocationHandler *answer_data_deallocation_handler   = NULL;
    
    if((worker_socket = zmq_socket(zmq_context,
                                   ZMQ_DEALER)) == NULL) {
        ZMQDIO_SRV_LAPP_ << "Error creating worker socket";
        return;
    }
    
    if((err = zmq_connect(worker_socket,
                          bind_inproc_url.c_str()))) {
        err = zmq_errno();
        
        ZMQDIO_SRV_LAPP_ << CHAOS_FORMAT("Error binding worker socket with error %1%[%2%]",%zmq_strerror(err)%err);
        return;
    }
    
    ZMQDIO_SRV_LAPP_ << "Entering in the thread loop for worker socket";
    while (run_server) {
        try {
            data_pack                           = NULL;
            data_pack_answer                    = NULL;
            answer_header_deallocation_handler  = NULL;
            answer_data_deallocation_handler    = NULL;
            
            if((err = reveiceDatapack(worker_socket,
                                      identity,
                                      &data_pack))) {
                CHK_AND_DELETE_OBJ_POINTER(data_pack);
                continue;
            } else {
                //   DEBUG_CODE(ZMQDIO_SRV_LAPP_ << "Received pack in " << PS_STR(priority_service) << " by " << identity;)
                //check if we need to sen an answer
                if((send_synchronous_answer = (bool)data_pack->header.dispatcher_header.fields.synchronous_answer)) {
                    //associate to the pointer the stack allocated data
                    data_pack_answer = &data_pack_answer_stack_alloc;
                    memset(data_pack_answer, 0, sizeof(DirectIODataPack));
                }
                
                //call handler
                err = DirectIOHandlerPtrCaller(handler_impl, delegate)(data_pack,
                                                                       data_pack_answer,
                                                                       &answer_header_deallocation_handler,
                                                                       &answer_data_deallocation_handler);
                if(send_synchronous_answer) {
                    
                    if((err = sendDatapack(worker_socket,
                                           identity,
                                           data_pack_answer,
                                           answer_header_deallocation_handler,
                                           answer_data_deallocation_handler))){
                        ZMQDIO_SRV_LAPP_ << "Error sending answer with code:" << err;
                    } else {
                        //anser si sent well
                    }
                    //alocation is not needed because it is created into the stack
                    //free(data_pack_answer);
                }
            }
            
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
    }
    ZMQDIO_SRV_LAPP_ << "Leaving the thread loop for worker socket";
    if((err = zmq_close(worker_socket))) {
        ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error closing worker socket with error %1%",%err);
    }
}

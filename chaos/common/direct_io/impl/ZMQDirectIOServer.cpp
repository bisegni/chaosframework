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


namespace chaos_data = chaos::common::data;

using namespace chaos::common::direct_io::impl;
using namespace chaos::common::direct_io;

DEFINE_CLASS_FACTORY(ZMQDirectIOServer, DirectIOServer);

ZMQDirectIOServer::ZMQDirectIOServer(std::string alias):DirectIOServer(alias) {
    zmq_context = NULL;
    priority_socket = NULL;
    service_socket = NULL;
    run_server = false;
};

ZMQDirectIOServer::~ZMQDirectIOServer(){
};

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
    DirectIOServer::start();
    run_server = true;
    
    //create the ZMQContext
    zmq_context = zmq_ctx_new();
    if(zmq_context == NULL) throw chaos::CException(0, "Error creating zmq context", __PRETTY_FUNCTION__);
    
    //et the thread number
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 2);
    ZMQDIO_SRV_LAPP_ << "ZMQ context created";
    
    //queue thread
    ZMQDIO_SRV_LAPP_ << "Allocating and binding priority socket to "<< priority_socket_bind_str;
    ZMQDIO_SRV_LAPP_ << "Allocating threads for manage the requests";
    try{
        server_threads_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOServer::worker, this, false)));
        server_threads_group.add_thread(new boost::thread(boost::bind(&ZMQDirectIOServer::worker, this, true)));
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

#define PS_STR(x) (x?"service":"priority")
void ZMQDirectIOServer::worker(bool priority_service) {
    int							linger				= 0;
    int							water_mark			= 1000;
    int							timeout				= 5000;
    void						*socket				= NULL;
    int							err					= 0;
    bool						send_synchronous_answer = false;
    std::string                 identity;
    DirectIODataPack			*data_pack			= NULL;
    DirectIODataPack            *data_pack_answer   = NULL;
    DirectIODataPack            data_pack_answer_stack_alloc;
    DirectIODeallocationHandler *answer_header_deallocation_handler = NULL;
    DirectIODeallocationHandler *answer_data_deallocation_handler   = NULL;
    
    ZMQDIO_SRV_LAPP_ << "Startup thread for " << PS_STR(priority_service);
    
    //allcoate the delegate for this thread
    DirectIOHandlerPtr delegate = priority_service?&DirectIOHandler::serviceDataReceived:&DirectIOHandler::priorityDataReceived;
    
    ZMQDIO_SRV_LAPP_ << "Allocating and binding " << PS_STR(priority_service) << " socket";
    socket = zmq_socket (zmq_context, ZMQ_ROUTER);
    err = zmq_setsockopt (socket, ZMQ_LINGER, &linger, sizeof(int));
    if(err) {
        std::string msg = boost::str(boost::format("Error Setting linget to %1% socket")%PS_STR(priority_service));
        ZMQDIO_SRV_LAPP_ << msg;
        return;
    }
    err = zmq_setsockopt (socket, ZMQ_RCVHWM, &water_mark, sizeof(int));
    if(err) {
        std::string msg = boost::str(boost::format("Error Setting watermark to %1% socket")%PS_STR(priority_service));
        ZMQDIO_SRV_LAPP_ << msg;
        return;
    }
    err = zmq_setsockopt (socket, ZMQ_SNDHWM, &water_mark, sizeof(int));
    if(err) {
        std::string msg = boost::str(boost::format("Error Setting watermark to %1% socket")%PS_STR(priority_service));
        ZMQDIO_SRV_LAPP_ << msg;
        return;
    }
    
    err = zmq_setsockopt (socket, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    if(err) {
        std::string msg = boost::str(boost::format("Error Setting ZMQ_SNDTIMEO to %1% socket")%PS_STR(priority_service));
        ZMQDIO_SRV_LAPP_ << msg;
        return;
    }
    
    err = zmq_setsockopt (socket, ZMQ_SNDTIMEO, &timeout, sizeof(int));
    if(err) {
        std::string msg = boost::str(boost::format("Error Setting ZMQ_RCVTIMEO to %1% socket")%PS_STR(priority_service));
        ZMQDIO_SRV_LAPP_ << msg;
        return;
    }
    
    if(priority_service) {
        err = zmq_bind(socket, priority_socket_bind_str.c_str());
    } else {
        err = zmq_bind(socket, service_socket_bind_str.c_str());
    }
    
    if(err) {
        std::string msg = boost::str( boost::format("Error binding %1% socket to  %2% ")%PS_STR(priority_service) % priority_socket_bind_str);
        ZMQDIO_SRV_LAPP_ << msg;
        return;
    }
    
    ZMQDIO_SRV_LAPP_ << "Entering in the thread loop for " << PS_STR(priority_service) << " socket";
    while (run_server) {
        try {
            data_pack                           = NULL;
            data_pack_answer                    = NULL;
            answer_header_deallocation_handler  = NULL;
            answer_data_deallocation_handler    = NULL;
            if((err = reveiceDatapack(socket,
                                      identity,
                                      &data_pack))) {
                //we have some error
                ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% receiving data", %err);
                continue;
            } else {
                //check if we need to sen an answer
                if((send_synchronous_answer = (bool)data_pack->header.dispatcher_header.fields.synchronous_answer)) {
                    //associate to the pointer the stack allocated data
                    data_pack_answer = &data_pack_answer_stack_alloc;
                    memset(data_pack_answer, 0, sizeof(DirectIODataPack));
                }
                
                //call handler
                if((err = DirectIOHandlerPtrCaller(handler_impl, delegate)(data_pack,
                                                                           data_pack_answer,
                                                                           &answer_header_deallocation_handler,
                                                                           &answer_data_deallocation_handler))) {
                    //we have some error
                    ZMQDIO_SRV_LERR_ << CHAOS_FORMAT("Error %1% calling directio handler", %err);
                    deleteDataWithHandler(answer_header_deallocation_handler,
                                          DisposeSentMemoryInfo::SentPartHeader,
                                          data_pack_answer->header.dispatcher_header.fields.channel_opcode,
                                          data_pack_answer->channel_header_data);
                    deleteDataWithHandler(answer_data_deallocation_handler,
                                          DisposeSentMemoryInfo::SentPartData,
                                          data_pack_answer->header.dispatcher_header.fields.channel_opcode,
                                          data_pack_answer->channel_data);
                } else {
                    if(send_synchronous_answer) {
                        if((err = sendDatapack(socket,
                                               identity,
                                               data_pack_answer,
                                               answer_header_deallocation_handler,
                                               answer_data_deallocation_handler))){
                            ZMQDIO_SRV_LAPP_ << "Error sending answer with code:" << err;
                        } else {
                            //anser si sent well
                        }
                        //alocation is not needed because it is created into the stack
                    } else {
                        deleteDataWithHandler(answer_header_deallocation_handler,
                                              DisposeSentMemoryInfo::SentPartHeader,
                                              data_pack_answer->header.dispatcher_header.fields.channel_opcode,
                                              data_pack_answer->channel_header_data);
                        deleteDataWithHandler(answer_data_deallocation_handler,
                                              DisposeSentMemoryInfo::SentPartData,
                                              data_pack_answer->header.dispatcher_header.fields.channel_opcode,
                                              data_pack_answer->channel_data);
                    }
                }
            }
            
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
    }
    ZMQDIO_SRV_LAPP_ << "Leaving the thread loop for " << (priority_service?"service":"priority") << " socket";
    ZMQDIO_SRV_LAPP_ << "Stopping priority socket";
    if(socket) {
        zmq_close(socket);
        socket = NULL;
    }
}

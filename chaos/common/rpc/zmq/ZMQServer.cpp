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

#include <chaos/common/global.h>
#include <chaos/common/rpc/zmq/ZMQServer.h>
#include <chaos/common/rpc/zmq/ZmqMemoryManagement.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/exception/exception.h>

#define ZMQS_LAPP INFO_LOG(ZMQServer)
#define ZMQS_LDBG DBG_LOG(ZMQServer)
#define ZMQS_LERR ERR_LOG(ZMQServer)
#define DEFAULT_MSGPACK_DISPATCHER_PORT             8888
#define DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER    4

using namespace std;
using namespace chaos;
using namespace boost;
using namespace chaos::common::data;

static void my_free (void *data, void *hint) {
    if(hint) {
        MemoryManagement *tmp = static_cast<MemoryManagement*>(hint);
        delete(tmp);
    } else {free(data);};
}

DEFINE_CLASS_FACTORY(ZMQServer, RpcServer);
ZMQServer::ZMQServer(const string& alias):
RpcServer(alias),
thread_number(0),
zmq_context(NULL),
run_server(false){
    
}

ZMQServer::~ZMQServer() {
    
}

//init the server getting the configuration value
void ZMQServer::init(void *init_data) throw(CException) {
    //get portnumber and thread number
    CDataWrapper *adapterConfiguration = reinterpret_cast<CDataWrapper*>(init_data);
    ZMQS_LAPP << "initialization";
    try {
        run_server = true;
        
        port_number = adapterConfiguration->getInt32Value(InitOption::OPT_RPC_SERVER_PORT);
        
        thread_number = adapterConfiguration->getInt32Value(InitOption::OPT_RPC_SERVER_THREAD_NUMBER);
        
        //bad patch OPT_RPC_DOMAIN_QUEUE_THREAD need to be removed
        if(thread_number  < adapterConfiguration->getUInt32Value(InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD)) {
            thread_number = adapterConfiguration->getUInt32Value(InitOption::OPT_RPC_DOMAIN_QUEUE_THREAD);
        }
        ZMQS_LAPP << "port number:" << port_number;
        ZMQS_LAPP << "worker thread number:" << thread_number;
        
        //create the zmq_context
        zmq_context = zmq_ctx_new();
        CHAOS_ASSERT(zmq_context)
        
        //et the thread number
        zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, 1);
        ZMQS_LAPP << "zmq_context initilized";
        
        bind_str << "tcp://*:";
        bind_str << port_number;
        ZMQS_LAPP << "bind url: "<<bind_str.str();
        
        //queue thread
        ZMQS_LAPP << "Allocating thread for manage the Workers";
        for (int idx = 0; idx<thread_number; idx++) {
            thread_group.add_thread(new boost::thread(boost::bind(&ZMQServer::worker, this)));
        }
        ZMQS_LAPP << "Worker threads initialized";
        thread_group.add_thread(new boost::thread(boost::bind(&ZMQServer::executeOnThread, this)));
        ZMQS_LAPP << "Receiver thread initialized";
        ZMQS_LAPP << "initialized";
    } catch (std::exception& e) {
        throw CException(-2, e.what(), "ZMQServer::init");
    } catch (...) {
        throw CException(-3, "generic error", "ZMQServer::init");
    }
}

//start the rpc adapter
void ZMQServer::start() throw(CException) {
}

//start the rpc adapter
void ZMQServer::stop() throw(CException) {
    
}

//deinit the rpc adapter
void ZMQServer::deinit() throw(CException) {
    run_server = false;
    ZMQS_LAPP << "Stopping thread";
    //wiath all thread
    zmq_ctx_shutdown(zmq_context);

    thread_group.join_all();
    zmq_ctx_destroy(zmq_context);

    ZMQS_LAPP << "Thread stopped";
}
#define ZMQ_DO_AGAIN(x) do{x}while(err == EAGAIN);
/*
 Thread method that work on buffer item
 */
void ZMQServer::executeOnThread(){
    int err = 0;
    ZMQS_LAPP << CHAOS_FORMAT("Entering pooler for %1%", %bind_str.str());
    void *receiver = zmq_socket (zmq_context, ZMQ_ROUTER);
    if(!receiver) return;
    
    void *proxy = zmq_socket (zmq_context, ZMQ_DEALER);
    if(!proxy) {
        zmq_close(proxy);
        return;
    }
    
    if((err = zmq_bind(receiver, bind_str.str().c_str()))) {
        ZMQS_LERR << boost::str(boost::format("zmq error %1% [%2%]")%err%zmq_strerror(zmq_errno()));
        zmq_close(receiver);
        zmq_close(proxy);
        return;
    } else if((err = zmq_bind(proxy, "inproc://workers"))) {
        ZMQS_LERR << boost::str(boost::format("zmq error %1% [%2%]")%err%zmq_strerror(zmq_errno()));
        zmq_close(receiver);
        zmq_close(proxy);
        return;
    }
    
    try {
        zmq_proxy(receiver, proxy, NULL);
    }catch (...) {}
    ZMQS_LAPP << CHAOS_FORMAT("Leaving pooler for %1%", %bind_str.str());
    zmq_close(receiver);
    zmq_close(proxy);
}

void ZMQServer::worker() {
    ZMQS_LAPP << CHAOS_FORMAT("Entering worker for %1%", %bind_str.str());
    ChaosUniquePtr<chaos::common::data::CDataWrapper> message_data;
    //data pack pointer
    int err = 0;
    int	linger = 500;
    int	water_mark = 10;
    
    void *receiver = zmq_socket (zmq_context, ZMQ_REP);
    if(!receiver) return;
    
    //err = zmq_bind(receiver, bind_str.str().c_str());
    err = zmq_connect(receiver, "inproc://workers");
    if(err == 0){
        ZMQS_LAPP << "Thread id:" << boost::lexical_cast<std::string>(boost::this_thread::get_id()) << "binded successfully";
    } else {
        ZMQS_LERR << "Thread id:" << boost::lexical_cast<std::string>(boost::this_thread::get_id()) << "binded with error";
        return;
    }
    err = zmq_setsockopt(receiver, ZMQ_LINGER, &linger, sizeof(int));
    if(err) {
        ZMQS_LERR << "Error setting ZMQ_LINGER value";
        return;
    }
    err = zmq_setsockopt(receiver, ZMQ_RCVHWM, &water_mark, sizeof(int));
    if(err) {
        ZMQS_LERR << "Error setting ZMQ_RCVHWM value";
        return;
    }
    err = zmq_setsockopt(receiver, ZMQ_SNDHWM, &water_mark, sizeof(int));
    if(err) {
        ZMQS_LERR << "Error setting ZMQ_SNDHWM value";
        return;
    }
    err = zmq_setsockopt(receiver, ZMQ_SNDTIMEO, &RpcConfigurationKey::GlobalRPCTimeoutinMSec, sizeof(int));
    if(err) {
        ZMQS_LERR << "Error setting ZMQ_SNDHWM value";
        return;
    }
    while (run_server) {
        try {
            zmq_msg_t request;
            zmq_msg_t response;
            
            err = zmq_msg_init(&request);
            
            ZMQS_LDBG << "Wait for message";
            err = zmq_recvmsg(receiver, &request, 0);
            if(run_server==0){
            	// no error should be issued on normal exit
                ZMQS_LDBG << "exiting from worker..";

            	continue;
            }

            if(err == -1 ) {
                int32_t sent_error = zmq_errno();
                std::string error_message = zmq_strerror(sent_error);
                ZMQS_LERR << "Error receiving message with code:" << sent_error << " message:" <<error_message;
            } else {
                if(zmq_msg_size(&request)>0) {
                    ZMQS_LDBG << "Message Received";
                    CDWShrdPtr result_data_pack;
                    message_data.reset(new CDataWrapper((const char*)zmq_msg_data(&request)));
                    //dispatch the command
                    if(message_data->hasKey("syncrhonous_call") &&
                       message_data->getBoolValue("syncrhonous_call")) {
                        result_data_pack.reset(command_handler->executeCommandSync(message_data.release()));
                    } else {
                        result_data_pack.reset(command_handler->dispatchCommand(message_data.release()));
                    }
                    //create zmq message
                    err = zmq_msg_init_data(&response, (void*)result_data_pack->getBSONRawData(), result_data_pack->getBSONRawSize(), my_free, new MemoryManagement(ChaosMoveOperator(result_data_pack)));
                    if(err == -1) {
                        //there was an error
                        int32_t sent_error = zmq_errno();
                        std::string error_message = zmq_strerror(sent_error);
                        ZMQS_LERR << "Error initializing the response message with code:" << sent_error << " message:" <<error_message;
                    } else {
                        //no error on create message
                        ZMQS_LDBG << "Send ack";
                        err = zmq_sendmsg(receiver, &response, ZMQ_NOBLOCK);
                        if(err == -1) {
                            int32_t sent_error = zmq_errno();
                            std::string error_message = zmq_strerror(sent_error);
                            ZMQS_LERR << "Error sending ack with code:" << sent_error << " message:" <<error_message;
                        }else {
                            ZMQS_LDBG << "ACK Sent";
                        }
                    }
                } else {
                    ZMQS_LDBG << "Empty message received";
                }
            }
            
            err = zmq_msg_close(&request);
            err = zmq_msg_close(&response);
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
    }
    zmq_close(receiver);
    ZMQS_LAPP << CHAOS_FORMAT("Leaving worker for %1%", %bind_str.str());
}

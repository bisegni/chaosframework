/*
 *	ZMQServer.cpp
 *	!CHOAS
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

#include <chaos/common/global.h>
#include <chaos/common/rpc/zmq/ZMQServer.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/exception/CException.h>

#define ZMQS_LAPP LAPP_ << "[ZMQServer] - "
#define ZMQS_LDBG LDBG_ << "[ZMQServer] - "
#define ZMQS_LERR LERR_ << "[ZMQServer] - "
#define DEFAULT_MSGPACK_DISPATCHER_PORT             8888
#define DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER    4

using namespace chaos;
using namespace chaos::common::data;
static void my_free (void *data, void *hint)
{
    delete (char*)data;
}

ZMQServer::ZMQServer(string alias):RpcServer(alias) {
    
}

ZMQServer::~ZMQServer() {
    
}

    //init the server getting the configuration value
void ZMQServer::init(void *init_data) throw(CException) {
        //get portnumber and thread number
	CDataWrapper *adapterConfiguration = reinterpret_cast<CDataWrapper*>(init_data);
    ZMQS_LAPP << "initialization";
    try {
        runServer = true;
        
        portNumber = adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT );
        
        threadNumber = adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER);
        
        ZMQS_LAPP << "port number:" << portNumber;
        ZMQS_LAPP << "thread number:" << threadNumber;
        
            //create the ZMQContext
        CHAOS_ASSERT(zmqContext = zmq_ctx_new())
        
            //et the thread number
        zmq_ctx_set(zmqContext, ZMQ_IO_THREADS, threadNumber);
        ZMQS_LAPP << "zmqContext initilized";
        
        
        bindStr << "tcp://*:";
        bindStr << portNumber;
        ZMQS_LAPP << "bind url: "<<bindStr.str();
        
        ZMQS_LAPP << "Workers initialized";
        ZMQS_LAPP << "initialized";
    } catch (std::exception& e) {
        throw CException(-2, e.what(), "ZMQServer::init");
    } catch (...) {
        throw CException(-3, "generic error", "ZMQServer::init");
    }
	runServer = true;
	//queue thread
	ZMQS_LAPP << "Allocating thread for manage the request";
	for (int idx = 0; idx<1; idx++) {
        threadGroup.add_thread(new thread(boost::bind(&ZMQServer::executeOnThread, this)));
    }
	ZMQS_LAPP << "Thread allocated";
}

    //start the rpc adapter
void ZMQServer::start() throw(CException) {
}

    //start the rpc adapter
void ZMQServer::stop() throw(CException) {
   
}

    //deinit the rpc adapter
void ZMQServer::deinit() throw(CException) {
	runServer = false;
    ZMQS_LAPP << "Stopping thread";
	zmq_ctx_shutdown(zmqContext);
	zmq_ctx_destroy(zmqContext);
	//wiath all thread
    threadGroup.join_all();
    ZMQS_LAPP << "Thread stopped";
}

/*
 Thread method that work on buffer item
 */
void ZMQServer::executeOnThread(){
    
        //data pack pointer
    int err = 0;
	int	linger = 500;
	int	water_mark = 100;
	int	send_timeout = 100;
    zmq_msg_t response;
	CDataWrapper *cdataWrapperPack = NULL;
	
	void *receiver = zmq_socket (zmqContext, ZMQ_REP);
    if(!receiver) return;
    
    err = zmq_bind(receiver, bindStr.str().c_str());
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
	err = zmq_setsockopt(receiver, ZMQ_SNDTIMEO, &send_timeout, sizeof(int));
	if(err) {
		ZMQS_LERR << "Error setting ZMQ_SNDHWM value";
		return;
	}
    while (runServer) {
        try {
            zmq_msg_t request;
            err = zmq_msg_init(&request);
			ZMQS_LDBG << "Start Receive";
            err = zmq_recvmsg(receiver, &request, 0);
            if(err == -1 || zmq_msg_size(&request)==0) {
				ZMQS_LDBG << "Received empty message";
				zmq_msg_close(&request);
                continue;
            }
			ZMQS_LDBG << "Received";
                //  Send reply back to client
                //dispatch the command
            cdataWrapperPack = commandHandler->dispatchCommand(new CDataWrapper((const char*)zmq_msg_data(&request)));
            
            
            auto_ptr<SerializationBuffer> result(cdataWrapperPack->getBSONData());
            result->disposeOnDelete = false;
            err = zmq_msg_init_data(&response, (void*)result->getBufferPtr(), result->getBufferLen(), my_free, NULL);
            ZMQS_LDBG << "Send ACK";
            err = zmq_sendmsg(receiver, &response, ZMQ_NOBLOCK);
			ZMQS_LDBG << "ACK Sent";
			err = zmq_msg_close(&request);
			
                //deallocate the data wrapper pack if it has been allocated
            if(cdataWrapperPack) delete(cdataWrapperPack);
			cdataWrapperPack = NULL;

        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
    }
    zmq_close(receiver);
}

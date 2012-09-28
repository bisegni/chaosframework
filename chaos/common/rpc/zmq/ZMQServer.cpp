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
#include <chaos/common/cconstants.h>
#include <chaos/common/exception/CException.h>

#define ZMQS_LAPP LAPP_ << "[ZMQServer] - "
#define DEFAULT_MSGPACK_DISPATCHER_PORT             8888
#define DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER    4

using namespace chaos;

    //init the server getting the configuration value
void ZMQServer::init(CDataWrapper *adapterConfiguration) throw(CException) {
        //get portnumber and thread number
    SetupStateManager::levelUpFrom(0, "ZMQServer already initialized");
    ZMQS_LAPP << "initialization";
    try {
        runServer = true;
        
        portNumber = adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT );
        
        threadNumber = adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER);
        
        ZMQS_LAPP << "port number:" << portNumber;
        ZMQS_LAPP << "thread number:" << threadNumber;
        
            //create the ZMQContext
        CHAOS_ASSERT(zmqContext = zmq_init (threadNumber));
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
}

    //start the rpc adapter
void ZMQServer::start() throw(CException) {
    SetupStateManager::levelUpFrom(1, "ZMQServer already started");
    ZMQS_LAPP << "Allocating thread for manage the request";
    runServer = true;
        //queue thread
    for (int idx = 0; idx<threadNumber; idx++) {
        threadGroup.add_thread(new thread(boost::bind(&ZMQServer::executeOnThread, this)));
    }
    
    ZMQS_LAPP << "Thread allocated";
    ZMQS_LAPP << "started";
    
}

    //start the rpc adapter
void ZMQServer::stop() throw(CException) {
    SetupStateManager::levelDownFrom(2, "ZMQServer already stopped");
    runServer = false;
    ZMQS_LAPP << "Stopping thread";
    
    boost::shared_lock<boost::shared_mutex> socketLock(socketMutex);
    for (int idx = 0; idx < socketsVector.size(); idx++) {
            //close the socket
        zmq_close(socketsVector[idx]);
    }
        //wiath all thread
    threadGroup.join_all();
    ZMQS_LAPP << "Thread stopped";
}

    //deinit the rpc adapter
void ZMQServer::deinit() throw(CException) {
    SetupStateManager::levelDownFrom(1, "ZMQServer already deinitialized");
        //serverThreadGroup.stopGroup(true);
    ZMQS_LAPP << "Deinitialization";
    zmq_term (zmqContext);
    ZMQS_LAPP << "Deinitialized";
}

/*
 Thread method that work on buffer item
 */
void ZMQServer::executeOnThread(){
    
        //data pack pointer
    int err = 0;
    zmq_msg_t message;
    void *receiver = zmq_socket (zmqContext, ZMQ_REP);
    if(!receiver) return;
    
    boost::shared_lock<boost::shared_mutex> socketLock(socketMutex);
    socketsVector.push_back(receiver);
    
    zmq_connect (receiver, "inproc://workers");
    CDataWrapper *cdataWrapperPack = NULL;
    while (runServer) {
        try {
            zmq_msg_init (&message);
            err = zmq_recvmsg(receiver, &message, 0);
            if(err == -1 || zmq_msg_size(&message)==0) {
                zmq_msg_close (&message);
                continue;
            }
            cdataWrapperPack = new CDataWrapper((const char*)zmq_msg_data(&message));
            
                //  Send reply back to client
                //dispatch the command
            cdataWrapperPack = commandHandler->dispatchCommand(cdataWrapperPack);
            
            auto_ptr<SerializationBuffer> result(cdataWrapperPack->getBSONData());
            zmq_send(receiver, (void*)result->getBufferPtr(), result->getBufferLen(), 0);
            
                //deallocate the data wrapper pack if it has been allocated
            if(cdataWrapperPack) delete(cdataWrapperPack);
        } catch (CException& ex) {
            DECODE_CHAOS_EXCEPTION(ex)
        }
        zmq_msg_close (&message);
    }
    zmq_close(receiver);
}

/*	
 *	ZMQClient.cpp
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
#ifdef DEV_WITH_ZMQ
#include <chaos/common/global.h>
#include <chaos/common/rpc/zmq/ZMQClient.h>
#include <chaos/common/chaos_constants.h>

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <msgpack/rpc/transport/udp.h>

using namespace chaos;
using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace msgpack;

#define ZMQC_LAPP LAPP_ << "[ZMQClient] " 

static void my_free (void *data, void *hint)
{
    delete (char*)data;
}


ZMQClient::ZMQClient(string *alias):RpcClient(alias){
};

ZMQClient::~ZMQClient(){
};

/*
 Initialization method for output buffer
 */
void ZMQClient::init(CDataWrapper *cfg) throw(CException) {
    ZMQC_LAPP << "initialization";
    int32_t threadNumber = cfg->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER)? cfg->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER):1;
    ZMQC_LAPP << "ObjectProcessingQueue<CDataWrapper> initialization with "<< threadNumber <<" thread";
    CObjectProcessingQueue<NetworkForwardInfo>::init(threadNumber);
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> initialized";
    
    ZMQC_LAPP << "ConnectionPool initialization";
    CHAOS_ASSERT(zmqContext = zmq_ctx_new())
    
        //et the thread number
    zmq_ctx_set(zmqContext, ZMQ_IO_THREADS, threadNumber);
    
    ZMQC_LAPP << "ConnectionPool initialized";
}

/*
 start the rpc adapter
 */
void ZMQClient::start() throw(CException) {
    
}

/*
 start the rpc adapter
 */
void ZMQClient::stop() throw(CException) {
    
}

/*
 Deinitialization method for output buffer
 */
void ZMQClient::deinit() throw(CException) {
    ZMQC_LAPP << "deinitialization";
    
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopping";
    CObjectProcessingQueue<NetworkForwardInfo>::clear();
    CObjectProcessingQueue<NetworkForwardInfo>::deinit();
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopped";
    
        //destroy the zmq context
    zmq_ctx_destroy(zmqContext);
    ZMQC_LAPP << "ZMQ Destroyed";

}

/*
 
 */
bool ZMQClient::submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(forwardInfo);
    ElementManagingPolicy ePolicy;
    try{
        if(!forwardInfo->destinationAddr.size())
            throw CException(0, "No destination ip in message description", "ZMQClient::submitMessage");
        if(!forwardInfo->message)
            throw CException(0, "No message in description", "ZMQClient::submitMessage");
            //allocate new forward info
            //submit action
        if(onThisThread){
            ePolicy.elementHasBeenDetached = false;
            processBufferElement(forwardInfo, ePolicy);
            delete(forwardInfo->message);
            delete(forwardInfo);
        } else {
            CObjectProcessingQueue<NetworkForwardInfo>::push(forwardInfo);
        }
    } catch(CException& ex){
            //in this case i need to delete the memory
        if(forwardInfo->message) delete(forwardInfo->message);
        if(forwardInfo) delete(forwardInfo);
            //in this case i need to delete te memory allocated by message
        DECODE_CHAOS_EXCEPTION(ex)
    }
    return true;
}

/*
 process the element action to be executed
 */
void ZMQClient::processBufferElement(NetworkForwardInfo *messageInfo, ElementManagingPolicy& elementPolicy) throw(CException) {
        //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    int err = 0;
    void *endPointSocket = zmq_socket (zmqContext, ZMQ_REQ);
        //this implementation is too slow, client for ip need to be cached
    if(!endPointSocket) throw CException(-1, "error allocating socket", "ZMQClient::processBufferElement");
    
        //get remote ip
        //serialize the call packet
    auto_ptr<chaos::SerializationBuffer> callSerialization(messageInfo->message->getBSONData());
    
    try{
        string url = "tcp://";
        url.append(messageInfo->destinationAddr);
        zmq_connect(endPointSocket, url.c_str());
        
        zmq_msg_t message;
            //detach buffer from carrier object so we don't need to copy anymore the data
        callSerialization->disposeOnDelete = false;
        err = zmq_msg_init_data(&message, (void*)callSerialization->getBufferPtr(), callSerialization->getBufferLen(), my_free, NULL);
        err = zmq_sendmsg(endPointSocket, &message, 0);
            //zmq_send(endPointSocket, callSerialization->getBufferPtr(), callSerialization->getBufferLen(), 0);
        
        zmq_msg_t reply;
        zmq_msg_init (&reply);
        zmq_recvmsg(endPointSocket, &reply, 0);
        
            //decode result of the posting message operation
        if(zmq_msg_size(&reply)>0){
#if DEBUG
            auto_ptr<CDataWrapper> resultBson(new CDataWrapper(static_cast<const char *>(zmq_msg_data(&reply))));
            LDBG_ << "Submission message result------------------------------";
            LDBG_ << resultBson->getJSONString();
            LDBG_ << "Submission message result------------------------------";
#endif
        }
        zmq_msg_close (&reply);
    } catch (msgpack::type_error& e) {
        ZMQC_LAPP << "Error during message forwarding:"<< e.what();
        return;
    } catch (std::exception& e) {
        ZMQC_LAPP << "Error during message forwarding:"<< e.what();
        return;
    }

}
#endif
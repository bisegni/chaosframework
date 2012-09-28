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
#include <chaos/common/global.h>
#include <chaos/common/rpc/zmq/ZMQClient.h>
#include <chaos/common/cconstants.h>

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
    CObjectProcessingQueue<RpcMessageForwardInfo>::init(threadNumber);
    ZMQC_LAPP << "ObjectProcessingQueue<RpcMessageForwardInfo> initialized";
    
    ZMQC_LAPP << "ConnectionPool initialization";
    CHAOS_ASSERT(zmqContext = zmq_init(threadNumber))
    ZMQC_LAPP << "ConnectionPool initialized";
}

/*
 start the rpc adapter
 */
void ZMQClient::start() throw(CException) {
    
}

/*
 Deinitialization method for output buffer
 */
void ZMQClient::deinit() throw(CException) {
    ZMQC_LAPP << "deinitialization";
    
    ZMQC_LAPP << "ObjectProcessingQueue<RpcMessageForwardInfo> stopping";
    CObjectProcessingQueue<RpcMessageForwardInfo>::clear();
    CObjectProcessingQueue<RpcMessageForwardInfo>::deinit();
    ZMQC_LAPP << "ObjectProcessingQueue<RpcMessageForwardInfo> stopped";
    
        //destroy the zmq context
    zmq_term(zmqContext);
    ZMQC_LAPP << "ZMQ Destroyed";

}

/*
 
 */
bool ZMQClient::submitMessage(string& destinationIpAndPort, CDataWrapper *message, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(message);
    ElementManagingPolicy ePolicy;
    RpcMessageForwardInfo *newForwardInfo = NULL;
    
    try{
        if(!message->hasKey( RpcActionDefinitionKey::CS_CMDM_REMOTE_HOST_IP))  
            throw CException(0, "No destination ip in message description", "ZMQClient::submitMessage");
        
            //allocate new forward info
        newForwardInfo = new RpcMessageForwardInfo();
        newForwardInfo->nodeNetworkInfo.ipPort = destinationIpAndPort;
        newForwardInfo->rpcMessage = message;
            //submit action
        if(onThisThread){
            ePolicy.elementHasBeenDetached = false;
            processBufferElement(newForwardInfo, ePolicy);
            if(message) delete(message);
            if(newForwardInfo) delete(newForwardInfo);
                //in this case i need to delete te memo
            message = NULL;
            newForwardInfo = NULL;
        } else {
            CObjectProcessingQueue<RpcMessageForwardInfo>::push(newForwardInfo);
        }
    } catch(CException& ex){
            //in this case i need to delete the memory
        if(message) delete(message);
            //in this case i need to delete te memory allocated by message
        DECODE_CHAOS_EXCEPTION(ex)
    }
    return true;
}

/*
 process the element action to be executed
 */
void ZMQClient::processBufferElement(RpcMessageForwardInfo *messageInfo, ElementManagingPolicy& elementPolicy) throw(CException) {
        //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    zmq_msg_t reply;
    void *requester = zmq_socket (zmqContext, ZMQ_REQ);
        //this implementation is too slow, client for ip need to be cached
    if(!requester) throw CException(-1, "error allocating socket", "ZMQClient::processBufferElement");
    
        //get remote ip
    string remoteHost = messageInfo->nodeNetworkInfo.ipPort;
        //serialize the call packet
    auto_ptr<chaos::SerializationBuffer> callSerialization(messageInfo->rpcMessage->getBSONData());
    
    try{
        string url = "tcp://";
        url.append(remoteHost);
        zmq_connect(requester, url.c_str());
        zmq_send(requester, callSerialization->getBufferPtr(), callSerialization->getBufferLen(), 0);
        
       
        zmq_msg_init (&reply);
        zmq_recvmsg(requester, &reply, 0);
        
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
        zmq_msg_close (&reply);
        return;
    } catch (std::exception& e) {
        ZMQC_LAPP << "Error during message forwarding:"<< e.what();
        zmq_msg_close (&reply);
        return;
    }
        //localSession.notify(CommandManagerConstant::CS_CMDM_RPC_TAG, rawMsg);
    zmq_close(requester);

}

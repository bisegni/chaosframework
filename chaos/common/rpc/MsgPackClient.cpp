    //
    //  MsgPackClient.cpp
    //  ControlSystemLib
    //
    //  Created by Bisegni Claudio on 09/08/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#include "../global.h"
#include "MsgPackClient.h"
#include "../cconstants.h"

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <msgpack/rpc/transport/udp.h>

using namespace chaos;
using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace msgpack;

MsgPackClient::MsgPackClient(string *alias):RpcClient(alias){
    connectionPolling = NULL;
};

MsgPackClient::~MsgPackClient(){
};

/*
 Initialization method for output buffer
 */
void MsgPackClient::init(CDataWrapper *cfg) throw(CException) {
    LAPP_ << "Msgpack RpcSender initialization";
    int32_t threadNumber = cfg->hasKey(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER)? cfg->getInt32Value(CommandManagerConstant::RpcAdapterConstant::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER):1;
    LAPP_ << "Msgpack RpcSender ObjectProcessingQueue<CDataWrapper> initialization with "<< threadNumber <<" thread";
    CObjectProcessingQueue<CDataWrapper>::init(threadNumber);
    LAPP_ << "Msgpack RpcSender ObjectProcessingQueue<CDataWrapper> initialized";
    
    LAPP_ << "Msgpack RpcSender ConnectionPool initialization";
    connectionPolling = new rpc::session_pool();//msgpack::rpc::udp_builder()
    if(!connectionPolling)
        throw CException(0, "Msgpack initialization error", "MsgPackClient::init");
    connectionPolling->start(threadNumber);
    LAPP_ << "Msgpack RpcSender ConnectionPool initialized";
}

/*
 start the rpc adapter
 */
void MsgPackClient::start() throw(CException) {
    
}

/*
 Deinitialization method for output buffer
 */
void MsgPackClient::deinit() throw(CException) {
    LAPP_ << "Msgpack Sender deinitialization";
    
    LAPP_ << "Msgpack Sender connectionPolling stopping";
    if(connectionPolling)connectionPolling->end();
    LAPP_ << "Msgpack Sender connectionPolling stopped";
    
    LAPP_ << "Msgpack Sender ObjectProcessingQueue<CDataWrapper> stopping";
    CObjectProcessingQueue<CDataWrapper>::clear();
    CObjectProcessingQueue<CDataWrapper>::deinit();
    LAPP_ << "Msgpack Sender ObjectProcessingQueue<CDataWrapper> stopped";
}

/*
 
 */
bool MsgPackClient::submitMessage(CDataWrapper *message, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(message);
    try{
        if(!message->hasKey( CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP))  
            throw CException(0, "No destination ip in message description", "MsgPackClient::submitMessage");
       
            //submit action
        if(onThisThread){
            processBufferElement(message);
        } else {
            CObjectProcessingQueue<CDataWrapper>::push(message);
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
void MsgPackClient::processBufferElement(CDataWrapper *message) throw(CException) {
        //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    vector<string> hostTokens;
    msgpack::type::raw_ref rawResult;
        //this implementation is too slow, client for ip need to be cached
    
        //get remote ip
    string remoteHost = message->getStringValue(CommandManagerConstant::CS_CMDM_REMOTE_HOST_IP);
        //split server and port
    algorithm::split(hostTokens, remoteHost, is_any_of(":"));
    
        //allocate the msgpack client
    rpc::session localSession = connectionPolling->get_session(msgpack::rpc::ip_address(hostTokens[0], lexical_cast<uint16_t>(hostTokens[1])));
        //msgpack::rpc::client msgPackClient(msgpack::rpc::udp_builder(), ));
    
        //serialize the call packet
    auto_ptr<chaos::SerializationBuffer> callSerialization(message->getBSONData());
    msgpack::type::raw_ref rawMsg(callSerialization->getBufferPtr() , (uint32_t)callSerialization->getBufferLen());
    try{
        rawResult = localSession.call(CommandManagerConstant::CS_CMDM_RPC_TAG, rawMsg).get<msgpack::type::raw_ref>();
    } catch(...) {
        LAPP_ << "Error during message forwarding";
        return;
    }
            //localSession.notify(CommandManagerConstant::CS_CMDM_RPC_TAG, rawMsg);
    if(rawResult.size>0){
#if DEBUG
        auto_ptr<CDataWrapper> resultBson(new CDataWrapper(rawResult.ptr));
        LDBG_ << "Submission message result------------------------------";
        LDBG_ << resultBson->getJSONString();
        LDBG_ << "Submission message result------------------------------";
            //LDBG_ << 
            //beed to be
            //receivedPacket.reset(new CDataWrapper(bsonResult.ptr));
#endif
    }
        //localSession.get_loop()->flush();
} 
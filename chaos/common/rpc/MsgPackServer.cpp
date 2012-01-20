//
//  MsgPachDispatcher.cpp
//  ChaosFramework
//
//  Created by Claudio Bisegni on 01/05/11.
//  Copyright 2011 INFN. All rights reserved.
//

#include <msgpack/rpc/server.h>
#include <msgpack/rpc/transport/udp.h>

#include "../global.h"
#include "MsgPackServer.h"
#include "../cconstants.h"
#include "../exception/CException.h"
using namespace chaos;

#define DEFAULT_MSGPACK_DISPATCHER_PORT             8888
#define DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER    4

namespace rpc {
    using namespace msgpack;
    using namespace msgpack::rpc;
}


//init the server getting the configuration value
void MsgPackServer::init(CDataWrapper *adapterConfiguration) throw(CException) {
    //get portnumber and thread number
    
    LAPP_ << "MsgPackServer initialization"; 
    
    portNumber = adapterConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT)?
                                                adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT ):
                                                DEFAULT_MSGPACK_DISPATCHER_PORT;
    
    threadNumber = adapterConfiguration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER)?
                                                adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER):
                                                DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER;
    
    LAPP_ << "MsgPackServer Configuration";
    LAPP_ << "MsgPackServer[cfg] port number:" << portNumber;
    LAPP_ << "MsgPackServer[cfg] thread number:" << threadNumber;
    
    //MsgPackRpcServer *server = new MsgPackRpcServer();
    //server->disp = this;
	//dp.reset(this);
	msgpackServer.serve(this );
    
	msgpackServer.listen("0.0.0.0", portNumber);
    LAPP_ << "MsgPackServer initialized";
}

//start the rpc adapter
void MsgPackServer::start() throw(CException) {
    LAPP_ << "MsgPackServer starting";
    msgpackServer.start(threadNumber);
    LAPP_ << "MsgPackServer started";
}

//deinit the rpc adapter
void MsgPackServer::deinit() throw(CException) {
    msgpackServer.close();
}

/*
 Dispatch the msgpack message, it provide the desetialization
 and serialization of bson message, in the case that DEFAULT_CHAS_MESSAGE_KEY
 method has been requested by client
 */
void MsgPackServer::dispatch(request req) {
    
        //data pack pointer
    CDataWrapper *CDataWrapperPack = NULL;
    try {
        std::string method;
        req.method().convert(&method);
        
        if(method == RpcActionDefinitionKey::CS_CMDM_RPC_TAG) {
                
            msgpack::type::tuple<msgpack::type::raw_ref> params;
            req.params().convert(&params);
            
            const msgpack::type::raw_ref msgReceived = params.get<0>();

            //the new CDataWrapper(msgReceived.ptr) must not be deallocated, it's deallocation
            //is managed byt dispatcher subsystem
            //CDataWrapperPack = new CDataWrapper(msgReceived.ptr);
                //if(!CDataWrapperPack) throw CException(0, "CDataWrapper memory allocation error", "MsgPackServer::dispatch");
             
                //dispatch the command
            CDataWrapperPack = commandDispatcher->dispatchCommand(new CDataWrapper(msgReceived.ptr));
            
            //serialize the result
            auto_ptr<SerializationBuffer> serialization(CDataWrapperPack->getBSONData());
            
            //inpack the bson result
            msgpack::type::raw_ref  msgResult(serialization->getBufferPtr() , (int32_t)serialization->getBufferLen());
            
            //send back the serialization
            req.result(msgResult);
        } else {
            req.error(msgpack::rpc::NO_METHOD_ERROR);
        }
    } catch (msgpack::type_error& e) {
        req.error(msgpack::rpc::ARGUMENT_ERROR);
    } catch (CException& ex) {
         DECODE_CHAOS_EXCEPTION(ex)
    } 
    
            //deallocate the data wrapper pack if it has been allocated
    if(CDataWrapperPack) delete(CDataWrapperPack);
}

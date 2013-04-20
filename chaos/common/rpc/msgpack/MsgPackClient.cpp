/*	
 *	MsgPackClient.cpp
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
#include <chaos/common/rpc/msgpack/MsgPackClient.h>
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

MsgPackClient::MsgPackClient(string *alias):RpcClient(alias){
    connectionPolling = NULL;
};

MsgPackClient::~MsgPackClient(){
};

/*
 Initialization method for output buffer
 */
void MsgPackClient::init(void *cfg) throw(CException) {
    
    CDataWrapper *configuration = static_cast<CDataWrapper*>(cfg);
    
    LAPP_ << "Msgpack RpcSender initialization";
    int32_t threadNumber = configuration->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER)? configuration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER):1;
    LAPP_ << "Msgpack RpcSender ObjectProcessingQueue<CDataWrapper> initialization with "<< threadNumber <<" thread";
    CObjectProcessingQueue<NetworkForwardInfo>::init(threadNumber);
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
 start the rpc adapter
 */
void MsgPackClient::stop() throw(CException) {
    
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
    CObjectProcessingQueue<NetworkForwardInfo>::clear();
    CObjectProcessingQueue<NetworkForwardInfo>::deinit();
    LAPP_ << "Msgpack Sender ObjectProcessingQueue<CDataWrapper> stopped";
}

/*
 
 */
bool MsgPackClient::submitMessage(NetworkForwardInfo* forwardInfo, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(forwardInfo)
    ElementManagingPolicy ePolicy;
    try{
        if(!forwardInfo->destinationAddr.size())
            throw CException(0, "No destination ip in message description", "MsgPackClient::submitMessage");
        if(!forwardInfo->message)
            throw CException(0, "No message in description", "MsgPackClient::submitMessage");
            //allocate new forward info
            //submit action
        if(onThisThread){
            ePolicy.elementHasBeenDetached = false;
            processBufferElement(forwardInfo, ePolicy);
            delete(forwardInfo->message);
            delete(forwardInfo);
                //in this case i need to delete te memo
        } else {
            CObjectProcessingQueue<NetworkForwardInfo>::push(forwardInfo);
        }
    } catch(CException& ex){
            //in this case i need to delete the memory
        if(forwardInfo->message) delete(forwardInfo->message);
        if(forwardInfo) delete(forwardInfo);
        DECODE_CHAOS_EXCEPTION(ex)
    }
    return true;
}

/*
 process the element action to be executed
 */
void MsgPackClient::processBufferElement(NetworkForwardInfo *messageInfo, ElementManagingPolicy& elementPolicy) throw(CException) {
        //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    vector<string> hostTokens;
    msgpack::type::raw_ref rawResult;
        //this implementation is too slow, client for ip need to be cached
        //split server and port
    algorithm::split(hostTokens, messageInfo->destinationAddr, is_any_of(":"));
    
        //allocate the msgpack client
    rpc::session localSession = connectionPolling->get_session(msgpack::rpc::ip_address(hostTokens[0], lexical_cast<uint16_t>(hostTokens[1])));
        //msgpack::rpc::client msgPackClient(msgpack::rpc::udp_builder(), ));
    
        //serialize the call packet
    auto_ptr<chaos::SerializationBuffer> callSerialization(messageInfo->message->getBSONData());
    msgpack::type::raw_ref rawMsg(callSerialization->getBufferPtr() , (uint32_t)callSerialization->getBufferLen());
    try{
        rawResult = localSession.call(RpcActionDefinitionKey::CS_CMDM_RPC_TAG, rawMsg).get<msgpack::type::raw_ref>();
    } catch (msgpack::type_error& e) {
        LAPP_ << "Error during message forwarding:"<< e.what();
        return;
    } catch (std::exception& e) {
        LAPP_ << "Error during message forwarding:"<< e.what();
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

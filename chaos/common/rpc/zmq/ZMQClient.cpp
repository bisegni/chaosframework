/*
 *	ZMQClient.cpp
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
#include <chaos/common/global.h>
#include <chaos/common/rpc/zmq/ZMQClient.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace chaos;
using namespace chaos::common::data;
using namespace chaos::common::pool;

using namespace std;
using namespace boost;
using namespace boost::algorithm;

#define ZMQC_LAPP INFO_LOG(ZMQClient)
#define ZMQC_LDBG DBG_LOG(ZMQClient)
#define ZMQC_LERR ERR_LOG(ZMQClient)

#define ZMQ_DO_AGAIN(x) do{x}while(err == EAGAIN);
#define ZMQ_SOCKET_MAINTENANCE_TIMEOUT (1000 * 30)
#define ZMQ_SOCKET_LIFETIME_TIMEOUT (1000 * 60)
//-------------------------------------------------------
DEFINE_CLASS_FACTORY(ZMQClient, RpcClient);

static void my_free (void *data, void *hint) {
    free(data);
}


ZMQClient::ZMQClient(const string& alias):
RpcClient(alias),
zmq_context(NULL),
zmq_timeout(1000){}

ZMQClient::~ZMQClient(){}

/*
 Initialization method for output buffer
 */
void ZMQClient::init(void *init_data) throw(CException) {
    CDataWrapper *cfg = reinterpret_cast<CDataWrapper*>(init_data);
    ZMQC_LAPP << "initialization";
    int32_t threadNumber = cfg->hasKey(InitOption::OPT_RPC_SERVER_THREAD_NUMBER)? cfg->getInt32Value(InitOption::OPT_RPC_SERVER_THREAD_NUMBER):1;
    ZMQC_LAPP << "ObjectProcessingQueue<CDataWrapper> initialization with "<< threadNumber <<" thread";
    CObjectProcessingQueue<NetworkForwardInfo>::init(threadNumber);
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> initialized";
    
    ZMQC_LAPP << "ConnectionPool initialization";
    zmq_context = zmq_ctx_new();
    CHAOS_ASSERT(zmq_context)
    
    //et the thread number
    zmq_ctx_set(zmq_context, ZMQ_IO_THREADS, threadNumber);
    
    if(GlobalConfiguration::getInstance()->getRpcImplKVParam().count("zmq_timeout")) {
        CHAOS_NOT_THROW(zmq_timeout = boost::lexical_cast<int>(GlobalConfiguration::getInstance()->getRpcImplKVParam()["zmq_timeout"]););
        
    }
    
    ZMQC_LAPP << "ConnectionPool initialized";
}

/*
 start the rpc adapter
 */
void ZMQClient::start() throw(CException) {
    int err = 0;
    //start timere after and repeat every one minut
    if((err = chaos::common::async_central::AsyncCentralManager::getInstance()->addTimer(this, ZMQ_SOCKET_MAINTENANCE_TIMEOUT, ZMQ_SOCKET_MAINTENANCE_TIMEOUT))) {LOG_AND_TROW(ZMQC_LERR, err, "Error adding timer")}
}

/*
 start the rpc adapter
 */
void ZMQClient::stop() throw(CException) {
    int err = 0;
    if((err = chaos::common::async_central::AsyncCentralManager::getInstance()->removeTimer(this))) {LOG_AND_TROW(ZMQC_LERR, err, "Error removing timer")}
}

/*
 Deinitialization method for output buffer
 */
void ZMQClient::deinit() throw(CException) {
    ZMQC_LAPP << "deinitialization";
    
    boost::shared_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    
    map_socket.clear();
    
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopping";
    CObjectProcessingQueue<NetworkForwardInfo>::clear();
    CObjectProcessingQueue<NetworkForwardInfo>::deinit();
    ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopped";
    
    //destroy the zmq context
    zmq_ctx_shutdown(zmq_context);
    zmq_ctx_destroy(zmq_context);
    ZMQC_LAPP << "ZMQ Destroyed";
}

/*
 
 */
bool ZMQClient::submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread) throw(CException) {
    CHAOS_ASSERT(forwardInfo);
    ElementManagingPolicy ePolicy;
    try{
        if(!forwardInfo->destinationAddr.size())
            throw CException(0, "No destination ip in message description", __PRETTY_FUNCTION__);
        if(!forwardInfo->hasMessage())
            throw CException(0, "No message in description", __PRETTY_FUNCTION__);
        //allocate new forward info
        //submit action
        if(onThisThread){
            ePolicy.elementHasBeenDetached = false;
            processBufferElement(forwardInfo, ePolicy);
            //delete(forwardInfo->message);
            delete(forwardInfo);
        } else {
            CObjectProcessingQueue<NetworkForwardInfo>::push(forwardInfo);
        }
    } catch(CException& ex){
        //in this case i need to delete the memory
        //if(forwardInfo->message) delete(forwardInfo->message);
        if(forwardInfo) delete(forwardInfo);
        //in this case i need to delete te memory allocated by message
        DECODE_CHAOS_EXCEPTION(ex)
    }
    return true;
}

ResourcePool<void*>::ResourceSlot *ZMQClient::getSocketForNFI(NetworkForwardInfo *nfi) {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    SocketMapIterator it = map_socket.find(nfi->destinationAddr);
    if(it != map_socket.end()){
        return it->second->getNewResource();
    } else {
        boost::shared_ptr< ResourcePool<void*> > socket_pool(new ResourcePool<void*>(nfi->destinationAddr, this));
        map_socket.insert(make_pair(nfi->destinationAddr, socket_pool));
        return socket_pool->getNewResource();
    }
}

void ZMQClient::releaseSocket(ResourcePool<void*>::ResourceSlot *socket_slot_to_release) {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    map_socket[socket_slot_to_release->pool_identification]->releaseResource(socket_slot_to_release);
}

void ZMQClient::deleteSocket(ResourcePool<void*>::ResourceSlot *socket_slot_to_release) {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    map_socket[socket_slot_to_release->pool_identification]->releaseResource(socket_slot_to_release,
                                                                             true);
}

//----resource pool handler-----
void* ZMQClient::allocateResource(const std::string& pool_identification,
                                  uint32_t& alive_for_ms,
                                  bool& success) {
    int err = 0;
    int linger = 0;
    int water_mark = 2;
    
    //set the alive time to one minute
    alive_for_ms = ZMQ_SOCKET_LIFETIME_TIMEOUT;
    
    //create zmq socket
    void *new_socket = zmq_socket (zmq_context, ZMQ_REQ);
    if(!new_socket) {
        return NULL;
    } else if ((err = zmq_setsockopt(new_socket, ZMQ_LINGER, &linger, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(new_socket, ZMQ_RCVHWM, &water_mark, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(new_socket, ZMQ_SNDHWM, &water_mark, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(new_socket, ZMQ_SNDTIMEO, &zmq_timeout, sizeof(int)))) {
    } else if ((err = zmq_setsockopt(new_socket, ZMQ_RCVTIMEO, &zmq_timeout, sizeof(int)))) {
    } else {
        string url = "tcp://";
        url.append(pool_identification);
        if((err = zmq_connect(new_socket, url.c_str()))) {
            ZMQC_LERR << "Error "<< err <<" connecting socket to " <<pool_identification;
        } else {
            DEBUG_CODE(ZMQC_LAPP << "New socket for "<<pool_identification;)
        }
    }
    
    if(err) {
        if(new_socket) {
            ZMQC_LERR << "Error during configuration of the socket for "<<pool_identification;
            zmq_close(new_socket);
            //reset socket
            new_socket = NULL;
        }
    }
    //return socket
    success = (socket != NULL);
    return new_socket;
}

void ZMQClient::deallocateResource(const std::string& pool_identification, void* resource_to_deallocate) {
    CHAOS_ASSERT(resource_to_deallocate)
    DEBUG_CODE(ZMQC_LAPP << "delete socket for "<<pool_identification;)
    zmq_close(resource_to_deallocate);
}

//-----timer handler------
void ZMQClient::timeout() {
    boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
    SocketMapIterator it = map_socket.begin();
    while(it != map_socket.end()){
        SocketMapIterator tmp_it = it++;
        tmp_it->second->maintenance();
        if( tmp_it->second->getSize() == 0 ) {
            DEBUG_CODE(ZMQC_LAPP << "Delete socket pool for:" << tmp_it->first;)
            map_socket.erase( tmp_it );
        }
    }
}

/*
 process the element action to be executed
 */
void ZMQClient::processBufferElement(NetworkForwardInfo *messageInfo, ElementManagingPolicy& elementPolicy) throw(CException) {
    //the domain is securely the same is is mandatory for submition so i need to get the name of the action
    int			err = 0;
    zmq_msg_t	reply;
    zmq_msg_t	message;
    zmq_msg_init (&reply);
    
    //get remote ip
    //serialize the call packet
    ResourcePool<void*>::ResourceSlot *socket_info = NULL;
    auto_ptr<chaos::common::data::SerializationBuffer> callSerialization(messageInfo->message->getBSONData());
    try{
        socket_info = getSocketForNFI(messageInfo);
        if(socket_info == NULL){
            ZMQC_LERR << "GetSocketForNFI failed";
            forwadSubmissionResultError(messageInfo,
                                        ErrorRpcCoce::EC_RPC_NO_SOCKET,
                                        "GetSocketForNFI failed",
                                        __PRETTY_FUNCTION__);
            return;
        }
        
        if(!socket_info->resource_pooled) {
            ZMQC_LERR << "Socket creation error";
            forwadSubmissionResultError(messageInfo,
                                        ErrorRpcCoce::EC_RPC_NO_SOCKET,
                                        "Socket creation error",
                                        __PRETTY_FUNCTION__);
            deleteSocket(socket_info);
            return;
        }
        
        //detach buffer from carrier object so we don't need to copy anymore the data
        callSerialization->disposeOnDelete = false;
        if((err = zmq_msg_init_data(&message, (void*)callSerialization->getBufferPtr(), callSerialization->getBufferLen(), my_free, NULL)) == -1) {
            int32_t sent_error = zmq_errno();
            std::string error_message =zmq_strerror(sent_error);
            ZMQC_LERR << "Error allocating zmq messagecode:" << sent_error << " message:" <<error_message;
            if(messageInfo->is_request) {
                forwadSubmissionResultError(messageInfo,
                                            (ErrorRpcCoce::EC_RPC_IMPL_ERR-1),
                                            "Error initializiend rcp message",
                                            __PRETTY_FUNCTION__);
            }
            //err = 0;
        } else {
            ZMQC_LDBG << "Try to send message";
            err = zmq_sendmsg(socket_info->resource_pooled, &message, ZMQ_DONTWAIT);
            if(err == -1) {
                int32_t sent_error = zmq_errno();
                std::string error_message = zmq_strerror(sent_error);
                ZMQC_LERR << "Error sending message with code:" << sent_error << " message:" <<error_message;
                if(messageInfo->is_request) {
                    forwadSubmissionResultError(messageInfo,
                                                ErrorRpcCoce::EC_RPC_SENDING_DATA,
                                                error_message,
                                                __PRETTY_FUNCTION__);
                }
                //delete socket
                deleteSocket(socket_info);
                socket_info = NULL;
            }else{
                ZMQC_LDBG << "Message sent now wait for ack";
                //ok get the answer
                err = zmq_recvmsg(socket_info->resource_pooled, &reply, 0);
                if(err == -1) {
                    int32_t sent_error = zmq_errno();
                    std::string error_message = zmq_strerror(sent_error);
                    ZMQC_LERR << "Error receiving ack for message message with code:" << sent_error << " message:" <<error_message;
                    if(messageInfo->is_request) {
                        forwadSubmissionResultError(messageInfo,
                                                    ErrorRpcCoce::EC_RPC_GETTING_ACK_DATA,
                                                    CHAOS_FORMAT("%1%[%2%]",%error_message%sent_error),
                                                    __PRETTY_FUNCTION__);
                    }
                    //delete socket
                    deleteSocket(socket_info);
                    socket_info = NULL;
                } else {
                    //decode result of the posting message operation
                    if(messageInfo->is_request) {
                        if(zmq_msg_size(&reply)>0){
                            ZMQC_LDBG << "ACK Received for request";
                            //there is a reply so we need to check if all ok or in case answer to request
                            forwadSubmissionResultError(messageInfo->sender_node_id,
                                                        messageInfo->sender_request_id,
                                                        new CDataWrapper(static_cast<const char *>(zmq_msg_data(&reply))));
                        } else {
                            ZMQC_LDBG << "Bad ACK received";
                            forwadSubmissionResultError(messageInfo,
                                                        ErrorRpcCoce::EC_RPC_GETTING_ACK_DATA,
                                                        "bad ack received",
                                                        __PRETTY_FUNCTION__);
                        }
                    } else {
                        ZMQC_LDBG << "ACK Received for message";
                    }
                }
            }
        }
        //return socket to the pool
        if(socket_info) {releaseSocket(socket_info);}
    }catch (std::exception& e) {
        ZMQC_LERR << "Error during message forwarding:"<< e.what();
    } catch (...) {
        ZMQC_LERR << "General error during message forwarding:";
    }
    zmq_msg_close (&message);
    zmq_msg_close (&reply);
}

uint64_t ZMQClient::getMessageQueueSize() {
    return CObjectProcessingQueue::queueSize();
}

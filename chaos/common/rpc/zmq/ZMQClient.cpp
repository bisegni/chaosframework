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
#include <chaos/common/chaos_constants.h>

#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

using namespace chaos;
using namespace chaos::common::data;

using namespace std;
using namespace boost;
using namespace boost::algorithm;

#define ZMQC_LAPP LAPP_ << "[ZMQClient] - "
#define ZMQC_LDBG LDBG_ << "[ZMQClient] - "
#define ZMQC_LERR LERR_ << "[ZMQClient] - "

DEFINE_CLASS_FACTORY(ZMQClient, RpcClient);

static void my_free (void *data, void *hint)
{
	delete (char*)data;
}


ZMQClient::ZMQClient(const string& alias):RpcClient(alias){
};

ZMQClient::~ZMQClient(){
};

/*
 Initialization method for output buffer
 */
void ZMQClient::init(void *init_data) throw(CException) {
	CDataWrapper *cfg = reinterpret_cast<CDataWrapper*>(init_data);
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
	
	boost::shared_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
	
	for(std::map<string, boost::shared_ptr<SocketInfo> >::iterator it = map_socket.begin();
		it != map_socket.end();
		it++) {
		boost::unique_lock<boost::shared_mutex> lock_socket(it->second->socket_mutex);
		
		if(it->second->socket) zmq_close(it->second->socket);
	}
	map_socket.clear();
	
	ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopping";
	CObjectProcessingQueue<NetworkForwardInfo>::clear();
	CObjectProcessingQueue<NetworkForwardInfo>::deinit();
	ZMQC_LAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopped";
	
	//destroy the zmq context
	zmq_ctx_shutdown(zmqContext);
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
			throw CException(0, "No destination ip in message description", __PRETTY_FUNCTION__);
		if(!forwardInfo->message)
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
		if(forwardInfo->message) delete(forwardInfo->message);
		if(forwardInfo) delete(forwardInfo);
		//in this case i need to delete te memory allocated by message
		DECODE_CHAOS_EXCEPTION(ex)
	}
	return true;
}

boost::shared_ptr<SocketInfo> ZMQClient::getSocketForNFI(NetworkForwardInfo *nfi) {
	int	err = 0;
	int linger = 500;
	int water_mark = 100;
	int timeout = 1000;
	boost::shared_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
	if(!map_socket.count(nfi->destinationAddr)) {
		ZMQC_LDBG << "Create new socket for " << nfi->destinationAddr;
		boost::shared_ptr<SocketInfo>  socket_info_ptr(new SocketInfo());
		socket_info_ptr->endpoint = nfi->destinationAddr;
		socket_info_ptr->socket = zmq_socket (zmqContext, ZMQ_REQ);
		//this implementation is too slow, client for ip need to be cached
		if(!socket_info_ptr->socket) {
			ZMQC_LERR << "Error creating socket";
			err = -1;
		} else if ((err = zmq_setsockopt(socket_info_ptr->socket, ZMQ_LINGER, &linger, sizeof(int)))) {
			ZMQC_LERR << "Error setting ZMQ_SNDHWM value";
		} else if ((err = zmq_setsockopt(socket_info_ptr->socket, ZMQ_RCVHWM, &water_mark, sizeof(int)))) {
			ZMQC_LERR << "Error setting ZMQ_SNDHWM value";
		} else if ((err = zmq_setsockopt(socket_info_ptr->socket, ZMQ_SNDHWM, &water_mark, sizeof(int)))) {
			ZMQC_LERR << "Error setting ZMQ_SNDHWM value";
		} else if ((err = zmq_setsockopt(socket_info_ptr->socket, ZMQ_SNDTIMEO, &timeout, sizeof(int)))) {
			ZMQC_LERR << "Error setting ZMQ_SNDTIMEO value";
		} else if ((err = zmq_setsockopt(socket_info_ptr->socket, ZMQ_RCVTIMEO, &timeout, sizeof(int)))) {
			ZMQC_LERR << "Error setting ZMQ_RCVTIMEO value";
		} else {
			string url = "tcp://";
			url.append(nfi->destinationAddr);
			if((err = zmq_connect(socket_info_ptr->socket, url.c_str()))) {
				ZMQC_LERR << "Error connectiong socket";
			}
		}
		
		if(err) {
			zmq_close(socket_info_ptr->socket);
			socket_info_ptr->socket = NULL;
			socket_info_ptr.reset();
		} else {
			map_socket.insert(make_pair(socket_info_ptr->endpoint, socket_info_ptr));
		}
		return socket_info_ptr;
	} else {
		ZMQC_LDBG << "return already allocated socket for " << nfi->destinationAddr;
		return map_socket[nfi->destinationAddr];
	}
}

void ZMQClient::disposeSocket(boost::shared_ptr<SocketInfo> socket_info_to_dispose) {
	boost::unique_lock<boost::shared_mutex> lock_socket_map(map_socket_mutex);
	map_socket.erase(socket_info_to_dispose->endpoint);
	zmq_close(socket_info_to_dispose->socket);
	socket_info_to_dispose->socket = NULL;
	socket_info_to_dispose.reset();
}

/*
 process the element action to be executed
 */
void ZMQClient::processBufferElement(NetworkForwardInfo *messageInfo, ElementManagingPolicy& elementPolicy) throw(CException) {
	//the domain is securely the same is is mandatory for submition so i need to get the name of the action
	int			err = 0;
	int32_t		result_forward_error = 0;
	zmq_msg_t	reply;
	zmq_msg_t	message;
	zmq_msg_init (&reply);
	
	//get remote ip
	//serialize the call packet
	boost::shared_ptr<SocketInfo> socket_info;
	auto_ptr<chaos::common::data::SerializationBuffer> callSerialization(messageInfo->message->getBSONData());
	try{
		socket_info = getSocketForNFI(messageInfo);
		//now we can use the socket
        boost::unique_lock<boost::shared_mutex> lock_socket(socket_info->socket_mutex);
		ZMQC_LDBG << "Lock acquired on socket mutex";
		
		if(!(socket_info.get() && socket_info->socket)) {
			ZMQC_LDBG << "Socket creation error";
			return;
		}
		
		//detach buffer from carrier object so we don't need to copy anymore the data
		callSerialization->disposeOnDelete = false;
		if((err = zmq_msg_init_data(&message, (void*)callSerialization->getBufferPtr(), callSerialization->getBufferLen(), my_free, NULL))) {
			throw CException(-1, "Error initializiend rcp message", __PRETTY_FUNCTION__);
		}else if(zmq_sendmsg(socket_info->socket, &message, ZMQ_NOBLOCK)==-1) {
			throw CException(-1, "Error sending data", __PRETTY_FUNCTION__);
		} else if(zmq_recvmsg(socket_info->socket, &reply, 0) == -1) {
			throw CException(-1, "Error receiving data", __PRETTY_FUNCTION__);
		} else {
			ZMQC_LDBG << "ACK Received";
			//decode result of the posting message operation
			if(zmq_msg_size(&reply)>0){
#if DEBUG
				std::stringstream ss;
				ss << "Sending result" << std::endl;
				auto_ptr<CDataWrapper> resultBson(new CDataWrapper(static_cast<const char *>(zmq_msg_data(&reply))));
				if(resultBson->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE)) {
					result_forward_error = resultBson->getInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE);
				}
				if(result_forward_error) {
					std::stringstream ss;
					ss << "Result error code: " << result_forward_error << std::endl;
					if(resultBson->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE)) {
						ss << "Result error message: " <<  resultBson->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE)<< std::endl;
					}
					if(resultBson->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN)) {
						ss << "Result error domain: " <<  resultBson->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN)<< std::endl;
					}
					ZMQC_LDBG << ss.str();
				}
#endif
			}
		}
	}catch (std::exception& e) {
		disposeSocket(socket_info);
		ZMQC_LAPP << "Error during message forwarding:"<< e.what();
		return;
	} catch (...) {
		disposeSocket(socket_info);
		ZMQC_LAPP << "General error during message forwarding:";
		return;
	}
	zmq_msg_close (&message);
	zmq_msg_close (&reply);
}
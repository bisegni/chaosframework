//
//  TCPUVClient.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/07/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include <chaos/common/rpc/tcpuv/TCPUVClient.h>

#define TCPUVClientLAPP LAPP_ << "[TCPUVClient] - "
#define TCPUVClientLDBG LDBG_ << "[TCPUVClient] - "
#define TCPUVClientLERR LERR_ << "[TCPUVClient] - "

using namespace chaos;

void TCPUVClient::on_close(uv_handle_t* handle) {
	free(handle);
}

void TCPUVClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf) {
	*buf = uv_buf_init((char*) malloc(suggested_size), (unsigned int)suggested_size);
}

void TCPUVClient::on_ack_read(uv_stream_t *server, ssize_t nread, const uv_buf_t* buf) {
	//delete the forward info class
	NetworkForwardInfo *forwardInfo = static_cast<NetworkForwardInfo*>(server->data);
	
	if (nread == -1) {
		TCPUVClientLERR << "error echo_read";
		delete(forwardInfo);
		//uv_close((uv_handle_t*) server, TCPUVClient::on_close);
		return;
	}
	
	auto_ptr<chaos::common::data::CDataWrapper> ack_result(new chaos::common::data::CDataWrapper(buf->base));
	LDBG_ << "TCPUVClient::on_ack_read message result------------------------------";
	LDBG_ << ack_result->getJSONString();
	LDBG_ << "TCPUVClient::on_ack_read message result------------------------------";
	free(buf->base);
	delete(forwardInfo);
	//uv_close((uv_handle_t*) server, TCPUVClient::on_close);
}

void TCPUVClient::on_write_end(uv_write_t *req, int status) {
	NetworkForwardInfo *forwardInfo = static_cast<NetworkForwardInfo*>(req->data);
	if (status) {
		TCPUVClientLERR << "error on_write_end ->" << status;
		delete(forwardInfo);
		return;
	}
	
	uv_read_start(req->handle, TCPUVClient::alloc_buffer, TCPUVClient::on_ack_read);
}

void TCPUVClient::on_connect(uv_connect_t *connection, int status) {
	NetworkForwardInfo *forwardInfo = static_cast<NetworkForwardInfo*>(connection->data);
	if (status) {
		TCPUVClientLERR << "error on_connect" << status;
		delete(forwardInfo);
		return;
	}
	uv_stream_t* stream = connection->handle;

	auto_ptr<chaos::common::data::SerializationBuffer> callSerialization(forwardInfo->message->getBSONData());
	callSerialization->disposeOnDelete = false;
	
	char buffer[100];
	uv_buf_t buf = uv_buf_init(buffer, sizeof(buffer));
	buf.len = callSerialization->getBufferLen();
	buf.base = (char*)(callSerialization->getBufferPtr());
	
	// ハンドルを取得
	stream->data = static_cast<void*>(forwardInfo);
	
	uv_write_t request;
	//int buf_count = 1;
	uv_write(&request, stream, &buf, 1, TCPUVClient::on_write_end);
}

TCPUVClient::TCPUVClient(string alias):RpcClient(alias), loop(NULL){
};

TCPUVClient::~TCPUVClient(){
};

/*
 init the rpc adapter
 */
void TCPUVClient::init(void *init_data) throw(CException) {
	chaos::common::data::CDataWrapper *cfg = reinterpret_cast<chaos::common::data::CDataWrapper*>(init_data);
	int32_t threadNumber = cfg->hasKey(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER)? cfg->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_THREAD_NUMBER):1;
	TCPUVClientLAPP << "ObjectProcessingQueue<CDataWrapper> initialization with "<< threadNumber <<" thread";
    CObjectProcessingQueue<NetworkForwardInfo>::init(threadNumber);
    TCPUVClientLAPP << "ObjectProcessingQueue<NetworkForwardInfo> initialized";
}

/*
 start the rpc adapter
 */
void TCPUVClient::start() throw(CException) {
	run = true;
	loop = uv_loop_new();
	uv_tcp_init(loop, &client);
	uv_tcp_keepalive(&client, 1, 60);
	loop_thread.reset(new boost::thread(boost::bind(&TCPUVClient::runLoop, this)));
}


/*
 start the rpc adapter
 */
void TCPUVClient::stop() throw(CException) {
	run = false;
	uv_stop(loop);
	uv_loop_delete(loop);
	loop_thread->join();
}


/*
 deinit the rpc adapter
 */
void TCPUVClient::deinit() throw(CException) {
	TCPUVClientLAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopping";
    CObjectProcessingQueue<NetworkForwardInfo>::clear();
    CObjectProcessingQueue<NetworkForwardInfo>::deinit();
    TCPUVClientLAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopped";
}

void TCPUVClient::runLoop() {
	//run the run loop
	while(run) {
		uv_run(loop, UV_RUN_DEFAULT);
		usleep(100);
	}
}

/*
 Submit the message to be send to a certain ip, the datawrapper must contains
 the key CS_CMDM_REMOTE_HOST_IP
 */
bool TCPUVClient::submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread) throw(CException) {
	CHAOS_ASSERT(forwardInfo);
    ElementManagingPolicy ePolicy;
	std::vector<std::string> server_desc_tokens;
    try{
        if(!forwardInfo->destinationAddr.size())
            throw CException(0, "No destination ip in message description", __PRETTY_FUNCTION__);
		
		boost::algorithm::split(server_desc_tokens, forwardInfo->destinationAddr, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
		if(server_desc_tokens.size() != 2)
			throw CException(0, "Invalid url format", __PRETTY_FUNCTION__);
		
        if(!forwardInfo->message)
            throw CException(0, "No message in description", "ZMQClient::submitMessage");
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

void TCPUVClient::processBufferElement(NetworkForwardInfo *messageInfo, ElementManagingPolicy& elementPolicy) throw(CException) {

	
	std::vector<std::string> server_desc_tokens;
	boost::algorithm::split(server_desc_tokens, messageInfo->destinationAddr, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	
	uv_connect_t* connect_req = (uv_connect_t*)malloc(sizeof(uv_connect_t));
	struct sockaddr_in req_addr;
	uv_ip4_addr(server_desc_tokens[0].c_str(), boost::lexical_cast<int>(server_desc_tokens[1]), &req_addr);
	
	//exec the connection
	elementPolicy.elementHasBeenDetached = true;
	connect_req->data = static_cast<void*>(messageInfo);
	uv_tcp_connect(connect_req, &client, (const struct sockaddr*)&req_addr, TCPUVClient::on_connect);

}
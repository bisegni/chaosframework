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

struct RequestInfo {
	uv_tcp_t *tcp_connection;
	NetworkForwardInfo *messageInfo;
	chaos::common::data::SerializationBuffer *serializationBuffer;
	uv_timer_t *phase_timer;
};

#define DEALLOCATE_REQUEST_INFO(x) \
if(x) { \
if(x->messageInfo) delete(x->messageInfo);\
if(x->serializationBuffer) delete(x->serializationBuffer);\
free(x);\
}

void TCPUVClient::timer_cb(uv_timer_t* handle) {
	RequestInfo *ri = static_cast<RequestInfo*>(handle->data);
	//uv_close((uv_handle_t*)ri->tcp_connection, TCPUVClient::on_close);
	//uv_close((uv_handle_t*)handle, TCPUVClient::on_close);
}

void TCPUVClient::on_close(uv_handle_t* handle) {
	LDBG_ << "TCPUVClient::on_close";
	free(handle);
}

void TCPUVClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf) {
	TCPUVClientLDBG << "alloc_buffer ";
	*buf = uv_buf_init((char*) malloc(suggested_size), (unsigned int)suggested_size);
}

void TCPUVClient::on_ack_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t* buf) {
	//delete the forward info class
	TCPUVClientLDBG << "on_ack_read " << nread;
	RequestInfo *ri = static_cast<RequestInfo*>(stream->data);
	stream->data = NULL;
	
	if (nread>0) {
		auto_ptr<chaos::common::data::CDataWrapper> ack_result(new chaos::common::data::CDataWrapper(buf->base));
		LDBG_ << "TCPUVClient::on_ack_read message result------------------------------";
		LDBG_ << ack_result->getJSONString();
		LDBG_ << "TCPUVClient::on_ack_read message result------------------------------";
		//uv_close((uv_handle_t*) server, TCPUVClient::on_close);
	}
	free(buf->base);
	uv_close((uv_handle_t*)stream, TCPUVClient::on_close);
	DEALLOCATE_REQUEST_INFO(ri)
}

void TCPUVClient::on_write_end(uv_write_t *req, int status) {
	TCPUVClientLDBG << "on_write_end " << status;
	RequestInfo *ri = static_cast<RequestInfo*>(req->data);
	if (status) {
		TCPUVClientLERR << "error on_write_end ->" << status;
		TCPUVClientLERR << "so we close the handle";
		if (!uv_is_closing((uv_handle_t*) req->handle))
			uv_close((uv_handle_t*)req->handle,TCPUVClient::on_close);
		free(req);
		DEALLOCATE_REQUEST_INFO(ri)
		return;
	}
	uv_read_start(req->handle, TCPUVClient::alloc_buffer, TCPUVClient::on_ack_read);
	free(req);
}

void TCPUVClient::on_connect(uv_connect_t *connection, int status) {
	TCPUVClientLDBG << "on_connect " << status;
	RequestInfo *ri = static_cast<RequestInfo*>(connection->data);
	if (status) {
		TCPUVClientLERR << "error on_connect" << uv_strerror(status);
		switch (status) {
			case ECONNREFUSED:
				TCPUVClientLERR << "ECONNREFUSED";
				break;
				
			default:
				break;
		}
		free(connection);
		DEALLOCATE_REQUEST_INFO(ri)
		return;
	}

	//get the data to send
	ri->serializationBuffer = ri->messageInfo->message->getBSONData();
	
	//initialize uv buffer
	uv_buf_t buf = uv_buf_init((char*)ri->serializationBuffer->getBufferPtr(), (unsigned int)ri->serializationBuffer->getBufferLen());
	
	uv_write_t *request = (uv_write_t*)malloc(sizeof(uv_write_t));
	uv_write(request, connection->handle, &buf, 1, TCPUVClient::on_write_end);
	free(connection);
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
	int err = 0;
	//run the run loop
	while(run) {
		err = uv_run(loop, UV_RUN_DEFAULT);
		usleep(1000);
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
            ePolicy.elementHasBeenDetached = true;
            processBufferElement(forwardInfo, ePolicy);
            //delete(forwardInfo->message);
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
	
	struct sockaddr_in req_addr;
	std::vector<std::string> server_desc_tokens;
	boost::algorithm::split(server_desc_tokens, messageInfo->destinationAddr, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	
	uv_ip4_addr(server_desc_tokens[0].c_str(), boost::lexical_cast<int>(server_desc_tokens[1]), &req_addr);

	RequestInfo * ri = (RequestInfo*)std::calloc(1, sizeof(RequestInfo));
	ri->tcp_connection = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
	uv_tcp_init(loop, ri->tcp_connection);
	uv_tcp_keepalive(ri->tcp_connection, 1, 60);

	elementPolicy.elementHasBeenDetached = true;
	ri->messageInfo = messageInfo;
	
	//exec the connection
	ri->tcp_connection->data = static_cast<void*>(ri);
	//ri->phase_timer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
	//ri->phase_timer->data = static_cast<void*>(ri);
	//init timer for timeout on operation
	//uv_timer_init(loop, ri->phase_timer);
	
	uv_connect_t *conn = (uv_connect_t*)malloc(sizeof(uv_connect_t));
	conn->data = ri->tcp_connection->data;
	uv_tcp_connect(conn, ri->tcp_connection, (const struct sockaddr*)&req_addr, TCPUVClient::on_connect);

}
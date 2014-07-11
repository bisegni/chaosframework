//
//  TCPUVClient.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/07/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/locks.hpp>

#include <chaos/common/rpc/tcpuv/TCPUVClient.h>

#include <sys/socket.h>
#include <netinet/in.h>

#define TCPUVClientLAPP LAPP_ << "[TCPUVClient] - "
#define TCPUVClientLDBG LDBG_ << "[TCPUVClient] - "
#define TCPUVClientLERR LERR_ << "[TCPUVClient] - (" << __LINE__ << ") - "

#define SLAB_SIZE 32000
using namespace chaos;

#define CLOESE_CONNECTION

#define DEALLOCATE_UV_CONNECTION_INFO(x) \
if(x->current_message_info) delete(x->current_message_info);\
if(x->current_serialization) delete(x->current_serialization);\
delete(x);


void TCPUVClient::on_close(uv_handle_t* handle) {
	TCPUVClientLDBG << "on_close";
	UVConnInfo *ci = static_cast<UVConnInfo*>(handle->data);
	DEALLOCATE_UV_CONNECTION_INFO(ci)
}

void TCPUVClient::shutdown_cb(uv_shutdown_t *req, int status) {
	//on_close((uv_handle_t*)req->handle);
	free(req);
}

void TCPUVClient::alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf) {
	TCPUVClientLDBG << "Alloc_buffer";
	*buf = uv_buf_init((char*) malloc(suggested_size), (unsigned int)suggested_size);
}

void TCPUVClient::on_ack_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t* buf) {
	ConnectionInfo *ci = static_cast<ConnectionInfo*>(stream->data);
	TCPUVClientLDBG << "STEP-2->on_ack_read " << nread;
	if (nread>0) {
		auto_ptr<chaos::common::data::CDataWrapper> ack_result(new chaos::common::data::CDataWrapper(buf->base));
		DEBUG_CODE(LDBG_ << "TCPUVClient::on_ack_read message result------------------------------";)
		DEBUG_CODE(LDBG_ << ack_result->getJSONString();)
		DEBUG_CODE(LDBG_ << "TCPUVClient::on_ack_read message result------------------------------";)
	} else {
		TCPUVClientLERR << "on_ack_read error " << uv_err_name((int)nread);
		boost::unique_lock<boost::shared_mutex> lock(ci->connection_mutex);
		
		stream->data = ci->uv_conn_info;
		ci->uv_conn_info = NULL;
		uv_close((uv_handle_t*)stream,TCPUVClient::on_close);
	}
	free(buf->base);
	
	//we need check othe data to send
	boost::unique_lock<boost::shared_mutex> lock(ci->connection_mutex);
	if(ci->queued_message_info.empty()) {
		DEBUG_CODE(TCPUVClientLDBG << "STEP-close connection for empty queue";)
		stream->data = ci->uv_conn_info;
		ci->uv_conn_info = NULL;
		uv_close((uv_handle_t*)stream,TCPUVClient::on_close);
	} else {
		DEBUG_CODE(TCPUVClientLDBG << "we have more data to send ->" << ci->queued_message_info.size();)
		//delete sent data
		if(ci->uv_conn_info->current_message_info) delete(ci->uv_conn_info->current_message_info);
		if(ci->uv_conn_info->current_serialization) delete(ci->uv_conn_info->current_serialization);
		TCPUVClient::send_data(ci, stream);
	}
	
	
}

void TCPUVClient::on_write_end(uv_write_t *req, int status) {
	TCPUVClientLDBG << "on_write_end" << status;
	int err = 0;
	if (status) {
		ConnectionInfo *ci = static_cast<ConnectionInfo*>(req->handle->data);
		boost::unique_lock<boost::shared_mutex> lock(ci->connection_mutex);
		TCPUVClientLERR << "error on_write_end ->" << uv_err_name(status);
		TCPUVClientLERR << "so we close the handle";
		req->handle->data = ci->uv_conn_info;
		ci->uv_conn_info = NULL;
		uv_close((uv_handle_t*)req->handle,TCPUVClient::on_close);
	} else {
		//Stop writing
		//uv_shutdown_t *shutdown_req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
		//uv_shutdown(shutdown_req, req->handle, TCPUVClient::shutdown_cb);
		//new read
		if((err = uv_read_start(req->handle, TCPUVClient::alloc_buffer, TCPUVClient::on_ack_read))){
			TCPUVClientLERR << "error on_write_end:uv_read_start" << uv_err_name(err);
		}
	}
	free(req);
}

void TCPUVClient::send_data(ConnectionInfo *ci, uv_stream_t *stream) {
	int err = 0;
	//get the next message to send
	ci->uv_conn_info->current_message_info = ci->queued_message_info.front();
	ci->queued_message_info.pop();
	
	//get next serialization to send
	ci->uv_conn_info->current_serialization = ci->uv_conn_info->current_message_info->message->getBSONData();
	
	//send data
	//uv_buf_t buffers[2];
	uv_write_t *new_write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
	
	ci->uv_conn_info->data_len = (uint64_t)ci->uv_conn_info->current_serialization->getBufferLen();
	
	TCPUVClientLDBG << "STEP-1->send_data of size " << ci->uv_conn_info->data_len;
	
	//buffers[0] = uv_buf_init((char*)&ci->uv_conn_info->data_len, sizeof(uint64_t));
	//buffers[1] = uv_buf_init((char*)ci->uv_conn_info->current_serialization->getBufferPtr(), (unsigned int)ci->uv_conn_info->current_serialization->getBufferLen());
	uv_buf_t buffers = uv_buf_init((char*)ci->uv_conn_info->current_serialization->getBufferPtr(), (unsigned int)ci->uv_conn_info->current_serialization->getBufferLen());
	if((err = uv_write(new_write_req, stream, &buffers, 1, TCPUVClient::on_write_end))) {
		TCPUVClientLERR << "error send_data:uv_write" << uv_err_name(err);
	}
}

void TCPUVClient::on_connect(uv_connect_t *connection, int status) {
	TCPUVClientLDBG << "on_connect " << status;
	ConnectionInfo *ci = static_cast<ConnectionInfo*>(connection->handle->data);
	boost::unique_lock<boost::shared_mutex> lock(ci->connection_mutex);
	if (status) {
		TCPUVClientLERR << "error on_connect" << uv_strerror(status);
		switch (status) {
			case ECONNREFUSED:
				TCPUVClientLERR << "ECONNREFUSED";
				break;
				
			default:
				break;
		}
		connection->handle->data = ci->uv_conn_info;
		ci->uv_conn_info = NULL;
		uv_close((uv_handle_t*)connection->handle,TCPUVClient::on_close);
	} else if(!ci->queued_message_info.empty()) {
		//lock access to connection info
		TCPUVClient::send_data(ci, connection->handle);
	} else {
		//we can close the conenction
		uv_close((uv_handle_t*)connection->handle,TCPUVClient::on_close);
	}
}

void TCPUVClient::async_shutdown_loop_cb(uv_async_t *handle) {
	uv_stop(handle->loop);
}

void TCPUVClient::async_submit_message_cb(uv_async_t *handle) {
	NetworkForwardInfo *message_info = NULL;
	ElementManagingPolicy element_policy;
	
	TCPUVClient *client_instance = static_cast<TCPUVClient*>(handle->data);
	while(client_instance->queue_async_submission.pop(message_info)) {
		if(message_info) {
			client_instance->processBufferElement(message_info, element_policy);
		}
	};
}

TCPUVClient::TCPUVClient(string alias):
RpcClient(alias),
queue_async_submission(1)/*, loop(NULL)*/ {
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
    //CObjectProcessingQueue<NetworkForwardInfo>::init(threadNumber);
    TCPUVClientLAPP << "ObjectProcessingQueue<NetworkForwardInfo> initialized";
}

/*
 start the rpc adapter
 */
void TCPUVClient::start() throw(CException) {
	run = true;
	
	//initlize loop
	uv_loop_init(&loop);
	
	//initialize async shutdown callback
	uv_async_init(&loop, &async_shutdown_loop, TCPUVClient::async_shutdown_loop_cb);
	
	//initlialize async submission calback
	async_submit_message.data = static_cast<void*>(this);
	uv_async_init(&loop, &async_submit_message, TCPUVClient::async_submit_message_cb);
	
	//start loop
	loop_thread.reset(new boost::thread(boost::bind(&TCPUVClient::runLoop, this)));
}


/*
 start the rpc adapter
 */
void TCPUVClient::stop() throw(CException) {
	run = false;
	
	uv_async_send(&async_shutdown_loop);
	loop_thread->join();
	
	for(map_addr_connection_iter iter = map_addr_connection_info.begin();
		iter != map_addr_connection_info.end();
		iter++) {
		delete(iter->second);
	}
}


/*
 deinit the rpc adapter
 */
void TCPUVClient::deinit() throw(CException) {
	TCPUVClientLAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopping";
	//  CObjectProcessingQueue<NetworkForwardInfo>::clear();
	//  CObjectProcessingQueue<NetworkForwardInfo>::deinit();
    TCPUVClientLAPP << "ObjectProcessingQueue<NetworkForwardInfo> stopped";
}

void TCPUVClient::runLoop() {
	//run the run loop
	int err = 0;
	//run the run loop
	TCPUVClientLAPP << "Enter loop";
	//boost::shared_lock<boost::shared_mutex> lock_loop(loop_mutex, boost::defer_lock);
	while(run) {
		//lock_loop.lock();
		err = uv_run(&loop, UV_RUN_DEFAULT);
		//lock_loop.unlock();
		//boost::this_thread::sleep(boost::posix_time::microseconds(250));
	}
	
	uv_loop_close(&loop);
	TCPUVClientLAPP << "Exit loop";
}

/*
 Submit the message to be send to a certain ip, the datawrapper must contains
 the key CS_CMDM_REMOTE_HOST_IP
 */
bool TCPUVClient::submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread) throw(CException) {
	CHAOS_ASSERT(forwardInfo);
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
        queue_async_submission.push(forwardInfo);
		
		//infrom uv to send
		uv_async_send(&async_submit_message);
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
	elementPolicy.elementHasBeenDetached = true;
	
	if(map_addr_connection_info.count(messageInfo->destinationAddr)) {
		//we have already a client but we need to se if it is allcoated
		sendToConnectionInfo(map_addr_connection_info[messageInfo->destinationAddr], messageInfo);
	} else {
		//we need a new client
		startConnection(messageInfo);
	}
	
}

bool TCPUVClient::startConnection(NetworkForwardInfo *message_info) {
	std::vector<std::string> server_desc_tokens;
	boost::algorithm::split(server_desc_tokens, message_info->destinationAddr, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
	ConnectionInfo * ci = new ConnectionInfo();
	
	//add new connection infointo map
	map_addr_connection_info.insert(make_pair(message_info->destinationAddr, ci));
	
	return sendToConnectionInfo(ci, message_info);
}

bool TCPUVClient::sendToConnectionInfo(ConnectionInfo *conenction_info, NetworkForwardInfo *message_info) {
	boost::unique_lock<boost::shared_mutex> lock_connection(conenction_info->connection_mutex);
	//
	conenction_info->queued_message_info.push(message_info);
	if(!conenction_info->uv_conn_info) {
		TCPUVClientLAPP << "Allocate new UVConnInfo";
		conenction_info->uv_conn_info  = new UVConnInfo();
		
		//i need to restart the connection
		//boost::unique_lock<boost::shared_mutex> lock_loop(loop_mutex);
		struct sockaddr_in req_addr;
		std::vector<std::string> server_desc_tokens;
		boost::algorithm::split(server_desc_tokens, message_info->destinationAddr, boost::algorithm::is_any_of(":"), boost::algorithm::token_compress_on);
		uv_ip4_addr(server_desc_tokens[0].c_str(), boost::lexical_cast<int>(server_desc_tokens[1]), &req_addr);
		
		
		uv_tcp_init(&loop, &conenction_info->uv_conn_info->tcp_connection);
		uv_tcp_keepalive(&conenction_info->uv_conn_info->tcp_connection, 1, 60);
        uv_tcp_nodelay(&conenction_info->uv_conn_info->tcp_connection, 1);
		//exec the connection
		conenction_info->uv_conn_info->tcp_connection.data = static_cast<void*>(conenction_info);
		uv_tcp_connect(&conenction_info->uv_conn_info->tcp_connection_request, &conenction_info->uv_conn_info->tcp_connection, (const struct sockaddr*)&req_addr, TCPUVClient::on_connect);
	}
	return true;
}
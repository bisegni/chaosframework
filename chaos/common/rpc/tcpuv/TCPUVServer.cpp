/*
 *	TCPUVServer.cpp
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
#include <chaos/common/rpc/tcpuv/TCPUVServer.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/exception/CException.h>

#include <chaos/common/utility/endianess.h>

#define TCPUVServerLAPP LAPP_ << "[TCPUVServer] - "
#define TCPUVServerLDBG LDBG_ << "[TCPUVServer] - "
#define TCPUVServerLERR LERR_ << "[TCPUVServer] - (" << __LINE__ << ") - "

#define DEFAULT_DISPATCHER_PORT             8888
#define DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER    4

#define TCPUV_AC(x) static_cast<TCPUVServerAcceptedConnection*>(x)

using namespace chaos;
using namespace chaos::common::data;

void TCPUVServer::on_close(uv_handle_t* handle) {
	TCPUVServerLDBG << "on_close ";
    //delete accepted connection info
    free(handle->data);
}

void TCPUVServer::on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	TCPUVServerLDBG << "on_alloc";
	switch(TCPUV_AC(handle->data)->receiving_phase) {
		case TCPUVServerConnectionReadPhaseHeader:
			TCPUVServerLDBG << "Allocate buffer for header of size " << (unsigned int)sizeof(uint64_t);
			*buf = uv_buf_init((char*) malloc(sizeof(uint64_t)), (unsigned int)sizeof(uint64_t));
			break;
			
		case TCPUVServerConnectionReadPhaseData:
			//TCPUVServerLDBG << "Allocate buffer for data of size " << (unsigned int)TCPUV_AC(handle->data)->phjase_one_data_size;
			//*buf = uv_buf_init((char*) malloc(TCPUV_AC(handle->data)->phjase_one_data_size), (unsigned int)TCPUV_AC(handle->data)->phjase_one_data_size);
			*buf = uv_buf_init((char*) malloc(suggested_size), (unsigned int)suggested_size);
			break;
	}
	
}

void TCPUVServer::on_write_end(uv_write_t *req, int status) {
	TCPUVServerLDBG << "on_write_end";
	int err = 0;
	//delete the sent data
	chaos::common::data::SerializationBuffer *ser = static_cast<chaos::common::data::SerializationBuffer*>(req->data);
	if(ser) delete(ser);
	
	if (status) {
		TCPUVServerLERR << "error on_write_end: " << status;
	}
	
	//read next message
	if((err = uv_read_start((uv_stream_t*)req->handle, TCPUVServer::on_alloc, TCPUVServer::on_read))) {
		TCPUVServerLERR << "Error on_write_end:uv_read_start" << uv_err_name(err);
	}
	
	free(req);
}

void TCPUVServer::_internal_read_header(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    DEBUG_CODE(TCPUVServerLDBG << "Received header: " <<  nread;)
	int err = 0;
    //read the ehader
    TCPUV_AC(handle->data)->phjase_one_data_size = FROM_LITTLE_ENDNS_NUM(uint64_t, *((uint64_t*)buf->base));
    
    if(TCPUV_AC(handle->data)->phjase_one_data_size) {
        TCPUV_AC(handle->data)->receiving_phase = TCPUVServerConnectionReadPhaseData;
		DEBUG_CODE(TCPUVServerLDBG << "We need to receive data for bytes-> " <<  TCPUV_AC(handle->data)->phjase_one_data_size;)
        //read the data
        if((err = uv_read_start(handle, TCPUVServer::on_alloc, TCPUVServer::on_read))) {
			TCPUVServerLERR << "Error _internal_read_header:uv_read_start" << uv_err_name(err);
		}
    } else {
		DEBUG_CODE(TCPUVServerLDBG << "No data we need to aspect so we close the connection " <<  nread;)
        // we have rceived zero length header that tell us to close connection
        uv_close((uv_handle_t*)handle, TCPUVServer::on_close);
    }
}

void TCPUVServer::_internal_read_data(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    DEBUG_CODE(TCPUVServerLDBG << "Received data: " <<  nread;)
    //read the message data
	int err = 0;
    chaos::common::data::CDataWrapper *action_result = TCPUV_AC(handle->data)->server_instance->handleReceivedData(buf->base, buf->len);
    
    chaos::common::data::SerializationBuffer *callSerialization = action_result->getBSONData();
    
    //delete the data not more needed
    delete(action_result);
    
    uv_write_t *write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    write_req->data = static_cast<void*>(callSerialization);
    uv_buf_t b = uv_buf_init((char *)callSerialization->getBufferPtr(), (unsigned int)callSerialization->getBufferLen());
    
    //write answer
    if((err = uv_write(write_req, handle, &b, 1, TCPUVServer::on_write_end))) {
		TCPUVServerLERR << "Error _internal_read_data:uv_write" << uv_err_name(err);
	}
    
    TCPUV_AC(handle->data)->receiving_phase = TCPUVServerConnectionReadPhaseData;
}

void TCPUVServer::on_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
	TCPUVServerLDBG << "on_read";
	if (nread > 0) {
        switch(TCPUV_AC(handle->data)->receiving_phase) {
            case TCPUVServerConnectionReadPhaseHeader: {
                DEBUG_CODE(TCPUVServerLDBG << "Received TCPUVServerConnectionReadPhaseHeader " <<  nread;)
                TCPUVServer::_internal_read_header(handle, nread, buf);
                break;
            }
                
            case TCPUVServerConnectionReadPhaseData: {
                DEBUG_CODE(TCPUVServerLDBG << "Received TCPUVServerConnectionReadPhaseData " <<  nread;)
                TCPUVServer::_internal_read_data(handle, nread, buf);
                break;
            }
        }
	} else if (nread < 0) {
		TCPUVServerLERR << "Error reading data:" << nread<< " "<< uv_strerror((int)nread);
		uv_close((uv_handle_t*)handle, TCPUVServer::on_close);
	} else {
		//all ok nothing read
		//we can close the stream
		DEBUG_CODE(TCPUVServerLDBG << "Close socket message";)
		uv_close((uv_handle_t*)handle, TCPUVServer::on_close);

	}
	//free the received data
	if(buf->base) free(buf->base);
}

void TCPUVServer::on_connected(uv_stream_t* server, int status) {
	TCPUVServerLDBG << "on_connected " << status;
	int err = 0;
	if (status == -1) {
		TCPUVServerLERR << "error on_connected: " << status;
		return;
	}
	
	TCPUVServerAcceptedConnection *ac = (TCPUVServerAcceptedConnection*)malloc(sizeof(TCPUVServerAcceptedConnection));
    ac->receiving_phase = TCPUVServerConnectionReadPhaseData;
	if ((err = uv_tcp_init(server->loop, &ac->tcp_connection))) {
        free(ac);
		TCPUVServerLERR << "Error initializing client on connection: " <<  uv_strerror(err);
		return;
	}
	ac->tcp_connection.data = static_cast<void*>(ac);
	//associate server to the client stream
	if ((err = uv_accept(server, (uv_stream_t*)&ac->tcp_connection))) {
		TCPUVServerLERR << "Error initializing client on connection: " <<  uv_strerror(err);
		uv_close((uv_handle_t*) &ac->tcp_connection, NULL);
		return;
	} else {
		//associate the server class instance to the client
        ac->server_instance = static_cast<TCPUVServer*>(server->data);
		uv_read_start((uv_stream_t*)&ac->tcp_connection, TCPUVServer::on_alloc, TCPUVServer::on_read);
	}
}

void TCPUVServer::async_shutdown_loop_cb(uv_async_t *handle) {
	if(!handle->data) return;
	uv_stop(handle->loop);
}

TCPUVServer::TCPUVServer(string alias):RpcServer(alias) {
    
}

TCPUVServer::~TCPUVServer() {
    
}

//init the server getting the configuration value
void TCPUVServer::init(void *init_data) throw(CException) {
	CDataWrapper *adapterConfiguration = reinterpret_cast<CDataWrapper*>(init_data);
	portNumber = adapterConfiguration->getInt32Value(RpcConfigurationKey::CS_CMDM_RPC_ADAPTER_TCP_UDP_PORT );
}

//start the rpc adapter
void TCPUVServer::start() throw(CException) {
	int err = 0;
    sockaddr_in addr;
    //init the uv loop
    uv_loop_init(&loop);
    
    //init the tcp socket
    uv_tcp_init(&loop, &server);
    
	//init shutdown async callback
	uv_async_init(&loop, &async_shutdown_loop, TCPUVServer::async_shutdown_loop_cb);

    //set the class instance into accept socket data field
	server.data = static_cast<void*>(this);
	
    //! puslish server on all ip
	uv_ip4_addr("0.0.0.0", portNumber, &addr);
	if ((err = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0))) {
		TCPUVServerLERR << "bind error " << uv_strerror(err);
		throw CException(-1, uv_strerror(err), __PRETTY_FUNCTION__);
	}
	
	//start listening...
	if ((err = uv_listen((uv_stream_t*)&server, 128, TCPUVServer::on_connected))) {
		TCPUVServerLERR << "listen error " << uv_strerror(err);
		throw CException(-2, uv_strerror(err), __PRETTY_FUNCTION__);
	}
	
    //start loop
	loop_thread.reset(new boost::thread(boost::bind(&TCPUVServer::runLoop, this)));
}

//start the rpc adapter
void TCPUVServer::stop() throw(CException) {
	async_shutdown_loop.data = (void*) this;
    //send shutdown async message
	uv_async_send(&async_shutdown_loop);
    
    //join on thread
	loop_thread->join();
}

//deinit the rpc adapter
void TCPUVServer::deinit() throw(CException) {
}

void TCPUVServer::runLoop() {
	int err = 0;
    TCPUVServerLAPP << "Starting loop thread " << err;
	err = uv_run(&loop, UV_RUN_DEFAULT);
	uv_loop_close(&loop);
	TCPUVServerLAPP << "Leaving loop thread " << err;
}

chaos::common::data::CDataWrapper * TCPUVServer::handleReceivedData(void *data, size_t len) {
	return commandHandler->dispatchCommand(new CDataWrapper((const char*)data));
}

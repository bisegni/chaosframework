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
#define TCPUVServerLERR LERR_ << "[TCPUVServer] - "

#define DEFAULT_DISPATCHER_PORT             8888
#define DEFAULT_MSGPACK_DISPATCHER_THREAD_NUMBER    4

#define AC(x) static_cast<AcceptedConnection*>(x)

using namespace chaos;
using namespace chaos::common::data;

void TCPUVServer::on_close(uv_handle_t* handle) {
	TCPUVServerLDBG << "on_close ";
    //delete accepted connection info
    free(handle->data);
}

void TCPUVServer::shutdown_connection_cb(uv_shutdown_t *req, int status) {
	TCPUVServerLDBG << "shutdown_connection_cb";
	if(status) {
		TCPUVServerLDBG << "shutdown_connection_cb error"<< status;
	}
	free(req);
}

void TCPUVServer::on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
	TCPUVServerLDBG << "on_alloc ";
	*buf = uv_buf_init((char*) malloc(suggested_size), (unsigned int)suggested_size);
}

void TCPUVServer::on_write_end(uv_write_t *req, int status) {
	TCPUVServerLDBG << "on_write_end ";
	
	//delete the sent data
	chaos::common::data::SerializationBuffer *ser = static_cast<chaos::common::data::SerializationBuffer*>(req->data);
	if(ser) delete(ser);
	
	if (status) {
		TCPUVServerLERR << "error on_write_end:" << status;
	}
	//uv_close((uv_handle_t*)req->handle, TCPUVServer::on_close);
	uv_shutdown_t *shutdown_req = (uv_shutdown_t*)malloc(sizeof(uv_shutdown_t));
	
	//Stop reading
	uv_shutdown(shutdown_req, req->handle, TCPUVServer::shutdown_connection_cb);
	
	//read next message
	uv_read_start((uv_stream_t*)req->handle, TCPUVServer::on_alloc, TCPUVServer::on_read);
	
	free(req);
}

void TCPUVServer::read_buffer(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    TCPUVServerLDBG << "Received header: " <<  nread;
    //read the ehader
    AC(handle->data)->phjase_one_data_size = FROM_LITTLE_ENDNS_NUM(uint64_t, *((uint64_t*)buf->base));
    free(buf->base);
    
    if(AC(handle->data)->phjase_one_data_size) {
        AC(handle->data)->receiving_phase = ConnectionReadPhaseData;
    
        //read the data
        uv_read_start(handle, TCPUVServer::on_alloc, TCPUVServer::on_read);
    } else {
        // we have rceived zero length header that tell us to close connection
        uv_close((uv_handle_t*)handle, TCPUVServer::on_close);
    }
}

void TCPUVServer::read_data(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    TCPUVServerLDBG << "Received data: " <<  nread;
    //read the message data

    chaos::common::data::CDataWrapper *action_result = AC(handle->data)->server_instance->handleReceivedData(buf->base, buf->len);
    
    chaos::common::data::SerializationBuffer *callSerialization = action_result->getBSONData();
    
    //delete the data not more needed
    delete(action_result);
    
    uv_write_t *write_req = (uv_write_t*)malloc(sizeof(uv_write_t));
    write_req->data = static_cast<void*>(callSerialization);
    uv_buf_t b = uv_buf_init((char *)callSerialization->getBufferPtr(), (unsigned int)callSerialization->getBufferLen());
    
    //write answer
    uv_write(write_req, handle, &b, 1, TCPUVServer::on_write_end);
    
    //delete the buffer
    free(buf->base);
    
    AC(handle->data)->receiving_phase = ConnectionReadPhaseHeader;
}

void TCPUVServer::on_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf) {
    
	TCPUVServerLDBG << "on_read ";
	if (nread > 0) {
        switch(AC(handle->data)->receiving_phase) {
            case ConnectionReadPhaseHeader: {
                TCPUVServerLDBG << "Received ConnectionReadPhaseHeader" <<  nread;
                TCPUVServer::read_buffer(handle, nread, buf);
                break;
            }
                
            case ConnectionReadPhaseData: {
                TCPUVServerLDBG << "Received ConnectionReadPhaseData" <<  nread;
                TCPUVServer::read_buffer(handle, nread, buf);
                break;
            }
        }
	} else if (nread < 0) {
		TCPUVServerLERR << "Error reading data:" << nread<< " "<< uv_strerror((int)nread);
		uv_close((uv_handle_t*)handle, TCPUVServer::on_close);
	} else {
		//all ok nothing read
		//we can close the stream
		TCPUVServerLDBG << "Close socket message";
		uv_close((uv_handle_t*)handle, TCPUVServer::on_close);

	}
	//free the received data
	if(buf->base) free(buf->base);
}

void TCPUVServer::on_connected(uv_stream_t* server, int status) {
	uv_tcp_t* client = NULL;
	TCPUVServerLDBG << "on_connected " << status;
	int err = 0;
	if (status == -1) {
		TCPUVServerLERR << "error on_connected:" << status;
		return;
	}
	
	AcceptedConnection *ac = (AcceptedConnection*)malloc(sizeof(AcceptedConnection));
    ac->receiving_phase = ConnectionReadPhaseHeader;
	if ((err = uv_tcp_init(server->loop, &ac->tcp_connection))) {
        free(ac);
		TCPUVServerLERR << "Error initializing client on connection: " <<  uv_strerror(err);
		return;
	}
	ac->tcp_connection.data = static_cast<void*>(ac);
	//associate server to the client stream
	if ((err = uv_accept(server, (uv_stream_t*)client))) {
		TCPUVServerLERR << "Error initializing client on connection: " <<  uv_strerror(err);
		uv_close((uv_handle_t*) &ac->tcp_connection, NULL);
		return;
	} else {
		//associate the server class instance to the client
        ac->server_instance = static_cast<TCPUVServer*>(server->data);
		uv_read_start((uv_stream_t*)client, TCPUVServer::on_alloc, TCPUVServer::on_read);
	}
}

TCPUVServer::TCPUVServer(string alias):RpcServer(alias), loop(NULL) {
    
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
	run = true;
	loop = uv_loop_new();
	uv_tcp_init(loop, &server);
	sockaddr_in addr;
	
	server.data = static_cast<void*>(this);
	
	uv_ip4_addr("0.0.0.0", portNumber, &addr);
	
	if ((err = uv_tcp_bind(&server, (const struct sockaddr*)&addr, 0))) {
		TCPUVServerLERR << "bind error " << uv_strerror(err);
		throw CException(-1, uv_strerror(err), __PRETTY_FUNCTION__);
	}
	
	
	if ((err = uv_listen((uv_stream_t*)&server, 128, TCPUVServer::on_connected))) {
		TCPUVServerLERR << "listen error " << uv_strerror(err);
		throw CException(-2, uv_strerror(err), __PRETTY_FUNCTION__);
	}
	
	loop_thread.reset(new boost::thread(boost::bind(&TCPUVServer::runLoop, this)));
}

//start the rpc adapter
void TCPUVServer::stop() throw(CException) {
	run = false;
	uv_close((uv_handle_t*)&server, NULL);
	uv_stop(loop);
	uv_loop_delete(loop);
	loop_thread->join();
}

//deinit the rpc adapter
void TCPUVServer::deinit() throw(CException) {
}

void TCPUVServer::runLoop() {
	int err = 0;
	//run the run loop
	while(run) {
		err = uv_run(loop, UV_RUN_DEFAULT);
		//TCPUVServerLDBG << err;
		usleep(1000);
	}
}

chaos::common::data::CDataWrapper * TCPUVServer::handleReceivedData(void *data, size_t len) {
	return commandHandler->dispatchCommand(new CDataWrapper((const char*)data));
}

//
//  TCPUVClient.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/07/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__TCPUVRpcClient__
#define __CHAOSFramework__TCPUVRpcClient__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/pqueue/ChaosProcessingQueue.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include <uv.h>

#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>

#include <queue>
#include <map>
namespace chaos {
    
	class TCPUVClient;
	
	struct UVConnInfo {
		//current forwarded data

		NetworkForwardInfo							*current_message_info;
		chaos::common::data::SerializationBuffer	*current_serialization;
		uint64_t									data_len;
		//
		uv_tcp_t			tcp_connection;
		uv_connect_t		tcp_connection_request;
	};
	
	struct ConnectionInfo {
		
		//libuv strucutre
		UVConnInfo *uv_conn_info;
		
		//data in quati to be forwarded
		std::queue<NetworkForwardInfo*> queued_message_info;
		
		//locking
		boost::shared_mutex	connection_mutex;
	};
    /*
     Class that manage the MessagePack message send.
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(TCPUVClient, RpcClient)/*, public CObjectProcessingQueue<NetworkForwardInfo>*/ {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(TCPUVClient)
        TCPUVClient(string alias);
        virtual ~TCPUVClient();
		
		uv_loop_t loop;
		uv_async_t async_shutdown_loop;
		uv_async_t async_submit_message;
		boost::lockfree::queue<NetworkForwardInfo *, boost::lockfree::fixed_sized<false> > queue_async_submission;
		//boost::shared_mutex					loop_mutex;
		boost::shared_ptr<boost::thread>	loop_thread;
		typedef std::map<std::string, ConnectionInfo*>::iterator	map_addr_connection_iter;
		std::map<std::string, ConnectionInfo*>						map_addr_connection_info;
		
		bool run;
		void runLoop();
		
		bool startConnection(NetworkForwardInfo *message_info);
		bool sendToConnectionInfo(ConnectionInfo *connection_info, NetworkForwardInfo *message_info);
    protected:
        virtual void processBufferElement(NetworkForwardInfo*, ElementManagingPolicy&) throw(CException);
		
		static void alloc_buffer(uv_handle_t *handle, size_t suggested_size, uv_buf_t* buf);
		
		static void on_ack_read(uv_stream_t *server, ssize_t nread, const uv_buf_t* buf);
		
		static void on_write_end(uv_write_t *req, int status);
		
		static void on_connect(uv_connect_t *connection, int status);
		
		static void on_close(uv_handle_t* handle);
		
		static void shutdown_cb(uv_shutdown_t *req, int status);
		
		static void send_data(ConnectionInfo *ci, uv_stream_t *stream);
		
		static void async_shutdown_loop_cb(uv_async_t *handle);
		
		static void async_submit_message_cb(uv_async_t *handle);
		//static void timer_cb(uv_timer_t* handle);
    public:
		
        /*
         init the rpc adapter
         */
        void init(void *init_data) throw(CException);
        
        /*
         start the rpc adapter
         */
        void start() throw(CException);
		
        /*
         start the rpc adapter
         */
        void stop() throw(CException);
        
        /*
         deinit the rpc adapter
         */
        void deinit() throw(CException);
        
        /*
         Submit the message to be send to a certain ip, the datawrapper must contains
         the key CS_CMDM_REMOTE_HOST_IP
         */
        bool submitMessage(NetworkForwardInfo *forwardInfo, bool onThisThread=false) throw(CException);
    };
}
#

#endif /* defined(__CHAOSFramework__TCPUVRpcClient__) */

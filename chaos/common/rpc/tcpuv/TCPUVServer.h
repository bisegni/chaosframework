//
//  TCPUVServer.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 01/07/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__TCPUVServer__
#define __CHAOSFramework__TCPUVServer__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <uv.h>

#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/utility/SetupStateManager.h>

namespace chaos {
    using namespace std;
	
	typedef enum ConnectionReadPhase {
		ConnectionReadPhaseHeader = 0,
		ConnectionReadPhaseData
	};
	
    class TCPUVServer;
    
	struct AcceptedConnection {
        TCPUVServer         *server_instance;
		uv_tcp_t            tcp_connection;
		//tag the receivement phase 0-header 1-data
		ConnectionReadPhase receiving_phase;
        
        //data len to expect
		uint64_t			phjase_one_data_size;
	};
	
    /*
     Class that implement the Chaos RPC adapter for 0mq protocoll
     */
    REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(TCPUVServer, RpcServer)  {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(TCPUVServer)

		friend void on_alloc_TCPUVServer(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
		
        bool run;
		uv_loop_t *loop;
		uv_tcp_t server;
		boost::scoped_ptr<boost::thread> loop_thread;
		
        TCPUVServer(string alias);
        virtual ~TCPUVServer();
		
		void runLoop();
	protected:
		static void on_write_end(uv_write_t *req, int status);
		
		static void shutdown_connection_cb(uv_shutdown_t *req, int status);
		
		static void on_close(uv_handle_t* handle);
		
		static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		
		static void on_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
		
        static void read_buffer(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
        
        static void read_data(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
        
		static void on_connected(uv_stream_t* s, int status);
		
		chaos::common::data::CDataWrapper *handleReceivedData(void *data, size_t len);
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
    };
    
}
#endif /* defined(__CHAOSFramework__TCPUVServer__) */

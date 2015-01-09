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

#include <uv.h>

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>


#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>


namespace chaos {
    using namespace std;
	
	typedef enum TCPUVServerConnectionReadPhase {
		TCPUVServerConnectionReadPhaseHeader = 0,
		TCPUVServerConnectionReadPhaseData
	} ConnectionReadPhase;
	
    class TCPUVServer;
    
	struct TCPUVServerAcceptedConnection {
        TCPUVServer						*server_instance;
		uv_tcp_t						tcp_connection;
		//tag the receivement phase 0-header 1-data
		TCPUVServerConnectionReadPhase	receiving_phase;
        
        //data len to expect
		uint64_t						phjase_one_data_size;
	};
	
    /*!
     Class that implement the Chaos RPC adapter using libuv library
     */
    DECLARE_CLASS_FACTORY(TCPUVServer, RpcServer)  {
        REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(TCPUVServer)
        //! uv dedicated loop for rpc server activity
		uv_loop_t loop;
        
        //!server accept socket
		uv_tcp_t server;
		uv_async_t async_shutdown_loop;
		boost::scoped_ptr<boost::thread> loop_thread;
		
        TCPUVServer(string alias);
        virtual ~TCPUVServer();
		
		void runLoop();
		
		//internal function doesn't deallocate the uv_buf_t base buffer
		static void _internal_read_header(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
        
        static void _internal_read_data(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
		
	protected:
        //! notify the completion of the write orpeation
		static void on_write_end(uv_write_t *req, int status);
		
        //! notify the end of the close oepration for an handle
		static void on_close(uv_handle_t* handle);
		
        //! notify the needs of the memory
		static void on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf);
		
        //! notify the completion of the data read phase
		static void on_read(uv_stream_t* handle, ssize_t nread, const uv_buf_t* buf);
        
        //! notify the tcp received connection
		static void on_connected(uv_stream_t* s, int status);
		
        //! porform the async shutdown of the uv loop
		static void async_shutdown_loop_cb(uv_async_t *handle);
		
        //! forward the received message  to the dispatcher and return the result of submission
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

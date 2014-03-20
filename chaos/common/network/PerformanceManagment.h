//
//  PerformanceManagment.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__PerformanceManagment__
#define __CHAOSFramework__PerformanceManagment__

#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/TemplatedKeyObjectContainer.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <string>
#include <map>

#define PERFORMANCE_MANAGMENT_RPC_DOMAIN "system:perf"

namespace chaos_data = chaos::common::data;
namespace chaos_direct_io = chaos::common::direct_io;

namespace chaos {
	//class declaration
	class NetworkBroker;
	namespace common {
		
		namespace direct_io {
			//class declaration
			class DirectIOClient;
			class DirectIOPerformanceSession;
		}
		
		namespace network {
			
			//! root point for all instance of managment node
			class PerformanceManagment :
			public chaos::utility::StartableService,
			public chaos::DeclareAction,
			public direct_io::DirectIOClientConnectionEventHandler,
			protected ::chaos::utility::TemplatedKeyObjectContainer<std::string, chaos_direct_io::DirectIOPerformanceSession*> {
				friend class chaos::NetworkBroker;
				chaos::NetworkBroker				*network_broker;
				
				boost::mutex						mutext_client_connection;
				chaos_direct_io::DirectIOClient		*global_performance_connection;
				
				bool work_on_purge;
				WaitSemaphore purge_wait_semaphore;
				boost::shared_ptr<boost::thread> thread_purge;
				
				boost::shared_mutex	mutex_map_purgeable;
				std::map<std::string, chaos_direct_io::DirectIOPerformanceSession*> map_purgeable_performance_node;
				
				PerformanceManagment(NetworkBroker *_network_broker);
				~PerformanceManagment();
				
				void  disposePerformanceNode(chaos_direct_io::DirectIOPerformanceSession *performance_node);
				void purge_map();
				void purge_worker();
				
				chaos_direct_io::DirectIOClient *getLocalDirectIOClientInstance();
			protected:
				//! Start the implementation
				void init(void *init_parameter) throw(chaos::CException);
								
				//! Start the implementation
				void start() throw(chaos::CException);
				
				//! Stop the implementation
				void stop() throw(chaos::CException);
				
				//! Stop the implementation
				void deinit() throw(chaos::CException);
				
				chaos_data::CDataWrapper* startPerformanceSession(chaos_data::CDataWrapper *param, bool& detach) throw(chaos::CException);
				
				chaos_data::CDataWrapper* stopPerformanceSession(chaos_data::CDataWrapper *param, bool& detach) throw(chaos::CException);
				
				void freeObject(std::string server_description, chaos_direct_io::DirectIOPerformanceSession *performance_node);
				
				void handleEvent(direct_io::DirectIOClientConnection *client_connection, direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);
			};
		}
	}
}

#endif /* defined(__CHAOSFramework__PerformanceManagment__) */

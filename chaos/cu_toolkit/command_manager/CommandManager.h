/*
 *	CommandManager.h
 *	!CHAOS
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

#ifndef CommandManager_H
#define CommandManager_H

#include <map>
#include <vector>
#include <map>
#include <string>
#include <boost/shared_ptr.hpp>
#include <chaos/common/global.h>
#include <chaos/common/rpc/RpcServer.h>
#include <chaos/common/rpc/RpcClient.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/exception/exception.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/cu_toolkit/command_manager/CommandManagerDefaultAdapters.h>


using namespace boost;

namespace chaos_data = chaos::common::data;

namespace chaos{
    namespace event {
        namespace channel {
            class InstrumentEventChannel;
        }
    }
    
    namespace cu {
		class ChaosCUToolkit;
		
		namespace data_manager {
			class DataManager;
		}
		namespace command_manager {
			/*
			 * CommandManager
			 * - Command Manager is the central class for the registration and execution of the custom command
			 *
			 */
			class CommandManager:
			public DeclareAction,
			public Configurable,
			public common::utility::Singleton<CommandManager>,
			public common::utility::StartableService {
				friend class data_manager::DataManager;
				friend class RpcAdapterRegister;
				friend class CommandDispatcherRegister;
				friend class ServerDelegator;
				friend class chaos::cu::ChaosCUToolkit;
				friend class common::utility::Singleton<CommandManager>;
				
				std::string metadataServerAddress;
				bool canUseMetadataServer;
				NetworkBroker *broker;
				
			public:
				/*
				 reference to the master pirv lib controller, for shutdown operation
				 by rpc
				 */
				ServerDelegator *server_handler;
				
				/*
				 * Initzialize the command manager
				 */
				void init(void *initParam) throw(CException);
				/*
				 * Deinitzialize the command manager
				 */
				void deinit() throw(CException);
				
				/*
				 * Start all sub process
				 */
				void start() throw(CException);
				
				//-----------------------
				void stop() throw(CException);
				
				/*
				 Configure the sandbox and all subtree of the CU
				 */
				chaos_data::CDataWrapper* updateConfiguration(chaos_data::CDataWrapper*);
				
				/*
				 Get MEtadataserver channel
				 */
                                chaos::common::message::MDSMessageChannel *getMetadataserverChannel();
                                
                                /*
                                 * get Network broker
                                 */
                                NetworkBroker *getNetworkBroker();

				
				//! delete an mds message channel
				void deleteMDSChannel(chaos::common::message::MDSMessageChannel *mds_channel);
				
				/*
				 Get Device event channel
				 */
				event::channel::InstrumentEventChannel *getInstrumentEventChannel();
				
				/*!
				 Delete an event channel allocated with command manager
				 */
				void deleteInstrumentEventChannel(event::channel::InstrumentEventChannel *eventChannel);
				
				/*
				 Register actions defined by AbstractActionDescriptor instance contained in the array
				 */
				void registerAction(DeclareAction*);
				/*
				 Deregister actions for a determianted domain
				 */
				void deregisterAction(DeclareAction*);
				
				/*
				 Shutdown the chaos control library
				 */
				chaos_data::CDataWrapper* shutdown(chaos_data::CDataWrapper*, bool&) throw (CException);
			private:
				CommandManager();
				~CommandManager();
			};
		}
    }
}
#endif

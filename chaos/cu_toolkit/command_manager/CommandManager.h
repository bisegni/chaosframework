/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
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
				void init(void *initParam);
				/*
				 * Deinitzialize the command manager
				 */
				void deinit();
				
				/*
				 * Start all sub process
				 */
				void start();
				
				//-----------------------
				void stop();
				
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
                chaos::common::data::CDWUniquePtr shutdown(chaos::common::data::CDWUniquePtr action_param) ;
			private:
				CommandManager();
				~CommandManager();
			};
		}
    }
}
#endif

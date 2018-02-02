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

#ifndef __CHAOSFramework__ChaosWANProxy__
#define __CHAOSFramework__ChaosWANProxy__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "global_type.h"
#include "persistence/DefaultPersistenceDriver.h"
#include "wan_interface/AbstractWANInterface.h"
#include "wan_interface/BaseWANInterfaceHandler.h"

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos {
	namespace wan_proxy {
		
		
		typedef std::vector<wan_interface::AbstractWANInterface*>			WanInterfaceList;
		typedef std::vector<wan_interface::AbstractWANInterface*>::iterator WanInterfaceListIterator;
		
		//! Chaos Node Directory base class
		/*!
		 
		 */
		class ChaosWANProxy :
		public ChaosCommon<ChaosWANProxy>,
		public ServerDelegator {
			friend class common::utility::Singleton<ChaosWANProxy>;
			
			static WaitSemaphore waitCloseSemaphore;
			
			chaos::common::utility::InizializableServiceContainer<chaos::wan_proxy::persistence::DefaultPersistenceDriver> persistence_driver;
			
			ChaosWANProxy();
			~ChaosWANProxy();
			static void signalHanlder(int);
			
			//! list of all active interface
			std::vector<wan_interface::AbstractWANInterface*> wan_active_interfaces;
			
			//!base handler pinter
			wan_interface::BaseWANInterfacelHandler *wan_interface_handler;
            common::utility::InizializableServiceContainer<external_command_pipe::ExternaCommandExecutor> external_cmd_executor;

		public:
			//gloabl applicaiton settin
			settings setting;
			
			typedef boost::mutex::scoped_lock lock;
			//! C and C++ attribute parser
			/*!
			 Specialized option for startup c and cpp program main options parameter
			 */
			void init(int argc, char* argv[]) throw (CException);
			//!stringbuffer parser
			/*
			 specialized option for string stream buffer with boost semantics
			 */
			void init(istringstream &initStringStream) throw (CException);
			void init(void *init_data)  throw(CException);
			void start()throw(CException);
			void stop()throw(CException);
			void deinit()throw(CException);
		};
	}
}
//#pragma GCC visibility pop
#endif

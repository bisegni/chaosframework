/*
 *	ChaosWANProxy.h
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

#ifndef __CHAOSFramework__ChaosWANProxy__
#define __CHAOSFramework__ChaosWANProxy__
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "global_type.h"
#include "ChaosBridge.h"
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
		public ServerDelegator,
		public common::utility::StartableService {
			friend class Singleton<ChaosWANProxy>;
			
			static WaitSemaphore waitCloseSemaphore;
			
			//! network broker for talk with others chaos node
			chaos::common::utility::StartableServiceContainer<NetworkBroker> network_broker_service;
			
			ChaosWANProxy();
			~ChaosWANProxy();
			static void signalHanlder(int);
			
			//! list of all active interface
			std::vector<wan_interface::AbstractWANInterface*> wan_active_interfaces;
			
			//!base handler pinter
			wan_interface::BaseWANInterfacelHandler *wan_interface_handler;
			
			ChaosBridge *chaos_bridge;
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

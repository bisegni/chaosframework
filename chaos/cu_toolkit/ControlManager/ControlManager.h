/*
 *	ControlManager.h
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
#ifndef ChaosFramework_ControlManager_h
#define ChaosFramework_ControlManager_h

#include <queue>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/thread/ChaosThread.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/async_central/async_central.h>
namespace chaos {
    
    namespace cu {
        using namespace std;
        using namespace boost;
        
        /*
         Manager for the Control Unit execution
         */
        class ControlManager :
		public DeclareAction,
		public chaos::utility::StartableService,
		public chaos::common::async_central::TimerHanlder,
		public Singleton<ControlManager> {
            friend class Singleton<ControlManager>;
            mutable boost::mutex qMutex;
            condition_variable lockCondition;
            queue< AbstractControlUnit* > submittedCUQueue;
            map<string, shared_ptr<AbstractControlUnit> > controlUnitInstanceMap;
            MDSMessageChannel *mdsChannel;
            
            int sendConfPackToMDS(CDataWrapper&);
            
            /*
             check for empty buffer
             */
            bool isEmpty() const;
            
			
            /*
             Constructor
             */
            ControlManager();
            
            /*
             Desctructor
             */
            ~ControlManager();
			
			void timeout();
        public:
            
            /*
             Initialize the Control Manager
             */
            void init(void *initParameter) throw(CException);
            
            /*
             Start the Control Manager
             */
            void start() throw(CException);
            
			void stop() throw(CException);
            /*
             Deinitialize the Control Manager
             */
            void deinit() throw(CException);
            
            /*
             Submit a new Control unit for operation
             */
            void submitControlUnit(AbstractControlUnit*) throw(CException);
            
            /*
             Init the sandbox
             */
            CDataWrapper* loadControlUnit(CDataWrapper*, bool&) throw (CException);
            
            /*
             Deinit the sandbox
             */
            CDataWrapper* unloadControlUnit(CDataWrapper*, bool&) throw (CException);
            
            /*
             Configure the sandbox and all subtree of the CU
             */
            CDataWrapper* updateConfiguration(CDataWrapper*, bool&);
        };
    }
}
#endif

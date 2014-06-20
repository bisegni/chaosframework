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
#include <map>

#include <boost/thread.hpp>
#include <boost/scoped_ptr.hpp>

#include <chaos/common/utility/Singleton.h>
#include <chaos/common/action/DeclareAction.h>
#include <chaos/common/exception/CException.h>
#include <chaos/common/general/Configurable.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/thread/WaitSemaphore.h>

#include <chaos/cu_toolkit/ControlManager/AbstractControlUnit.h>

namespace chaos {
    namespace cu {
        using namespace std;
        using namespace boost;
        
			//!Manager for the Control Unit managment
        /*!
			This class is the central point qhere control unit are managed. Two api are exposed 
			via RPC to load and unload the control unit giving some parameter.
         */
        class ControlManager :
		public DeclareAction,
		public chaos::utility::StartableService,
		public Singleton<ControlManager> {
            friend class Singleton<ControlManager>;
            mutable boost::shared_mutex mutex_registration;
			
			bool thread_run;
			chaos::WaitSemaphore thread_waith_semaphore;
			boost::scoped_ptr<boost::thread> thread_registration;
			
			//! metadata server channel for control unit registration
			MDSMessageChannel *mdsChannel;
			
			//!queue for control unit waiting to be published
            queue< AbstractControlUnit* > submittedCUQueue;
			
			//! control unit instance mapped with their unique identifier
            map<string, shared_ptr<AbstractControlUnit> > map_control_unit_instance;
           
			//! association by alias and control unit instancer
			std::map<string, chaos::common::utility::ObjectInstancer<AbstractControlUnit>* > map_cu_alias_instancer;
			
			//----------private method-----------
			//! send register control unit to the mds.
            int sendConfPackToMDS(CDataWrapper&);
			
            /*
             Constructor
             */
            ControlManager();
            
            /*
             Desctructor
             */
            ~ControlManager();
		protected:
			//! timer fire method
			void manageControlUnit();
        public:
            
            /*
             Initialize the Control Manager
             */
            void init(void *initParameter) throw(CException);
            
            /*
             Start the Control Manager
             */
            void start() throw(CException);
            
			
			/*!
			 Stop the control manager
			 */
			void stop() throw(CException);
			
            /*
             Deinitialize the Control Manager
             */
            void deinit() throw(CException);
            
            /*
             Submit a new Control unit instance
             */
            void submitControlUnit(AbstractControlUnit *control_unit_instance) throw(CException);
            
				//! control unit registration
			/*!
			 Register a control unit instancer associating it to an alias
			 \param control_unit_alias the alias associated to the tempalte class identification
			 */
			template<typename ControlUnitClass>
			void registerControlUnit(const std::string& control_unit_alias) {
				map_cu_alias_instancer.insert(make_pair(control_unit_alias, ALLOCATE_INSTANCER(ControlUnitClass, AbstractControlUnit)));
			}
			
			/*
			 Load the requested control unit
			 */
            CDataWrapper* loadControlUnit(CDataWrapper*, bool&) throw (CException);
            
			/*
			 Unload the requested control unit
			 The unload operation, check that the target control unit is in deinit state
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

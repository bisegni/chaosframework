/*
 *	ChaosMetadataService.h
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

#ifndef ChaosMetadataService_H
#define ChaosMetadataService_H
#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "mds_constants.h"
#include "mds_types.h"
#include "api/ApiManagment.h"
#include "cron_job/MDSCronousManager.h"

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos {
    namespace metadata_service {
            //! Chaos Node Directory base class
        /*!
         
         */
        class ChaosMetadataService :
		public ChaosCommon<ChaosMetadataService>,
		public ServerDelegator {
            friend class common::utility::Singleton<ChaosMetadataService>;
            
            static WaitSemaphore waitCloseSemaphore;
			
            //! subsystem needed by the api
            ApiSubserviceAccessor api_subsystem_accessor;
            
			//!persistence driver instance
			common::utility::InizializableServiceContainer<api::ApiManagment> api_managment_service;

			
            
            ChaosMetadataService(){};
            ~ChaosMetadataService(){};
            static void signalHanlder(int);
            
			//! convert param_key to a string of string hash map
			void fillKVParameter(std::map<std::string, std::string>& kvmap,
								 const std::vector<std::string>& multitoken_param);
        public:			
			struct setting	setting;

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

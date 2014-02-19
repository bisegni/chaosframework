
/*
 *	ChaosDataService.h
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

#ifndef __CHAOSFramework__ChaosDataService__
#define __CHAOSFramework__ChaosDataService__

#pragma GCC diagnostic ignored "-Woverloaded-virtual"

#include "dataservice_global.h"
#include "DataConsumer.h"
#include "worker/DataWorker.h"
#include "cache_system/CacheDriver.h"

#include <string>
#include <vector>

#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/utility/StartableService.h>


namespace common_utility = chaos::utility;

namespace chaos{
    namespace data_service {

		
        //! Chaos Data Service singleton
        /*!
         */
        class ChaosDataService : public ChaosCommon<ChaosDataService>, public common_utility::StartableService {
            friend class Singleton<ChaosDataService>;
            
            static WaitSemaphore waitCloseSemaphore;
            
            ChaosDataService();
            ~ChaosDataService();
            static void signalHanlder(int);
			
			utility::StartableServiceContainer<chaos::NetworkBroker> *network_broker;
            utility::StartableServiceContainer<DataConsumer> *data_consumer;
			utility::InizializableServiceContainer<DirectIOClient> *client;
        public:
			//----------setting----------
			ChaosDataServiceSetting settings;
			
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
            void start() throw(CException);;
            void stop() throw(CException);;
            void deinit() throw(CException);;
        };
    }
}

#endif /* defined(__CHAOSFramework__ChaosDataProxy__) */

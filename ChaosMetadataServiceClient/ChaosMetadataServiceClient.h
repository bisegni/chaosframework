/*
 *	MetadataServiceClient.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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
#ifndef __CHAOSFramework__MetadataServiceClient__
#define __CHAOSFramework__MetadataServiceClient__


#include <boost/thread/condition.hpp>

#include <chaos/common/global.h>
#include <chaos/common/ChaosCommon.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/thread/WaitSemaphore.h>
#include <chaos/common/utility/StartableService.h>

namespace chaos {
    namespace metadata_service_client {
        //! Chaos Node Directory base class
        /*!
         
         */
        class ChaosMetadataServiceClient :
        public ChaosCommon<ChaosMetadataServiceClient>,
        public ServerDelegator,
        public common::utility::StartableService {
            friend class common::utility::Singleton<ChaosMetadataServiceClient>;
            
            //!network broker service
            common::utility::StartableServiceContainer<chaos::common::network::NetworkBroker> network_broker_service;
            
            //!default constructor
            ChaosMetadataServiceClient(){};
            
            //! default destructor
            ~ChaosMetadataServiceClient(){};
        public:
            
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

#endif /* defined(__CHAOSFramework__MetadataServiceClient__) */

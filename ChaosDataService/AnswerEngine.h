//
//  AnswerEngine.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/02/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//

#ifndef __CHAOSFramework__AnswerEngine__
#define __CHAOSFramework__AnswerEngine__

#include "dataservice_global.h"


#include <map>
#include <string>
#include <stdint.h>

#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/channel/DirectIODeviceClientChannel.h>
#include <chaos/common/network/NetworkBroker.h>

using namespace chaos::utility;
using namespace chaos::common::direct_io;
using namespace chaos::common::direct_io::channel;


namespace chaos{
    namespace data_service {
		class ChaosDataService;
		
		class AnswerEngine : public InizializableService {
			friend class ChaosDataService;
			
			
			std::map<uint32_t, utility::InizializableServiceContainer<DirectIOClient> *> map_client;
			std::map<uint32_t, DirectIODeviceClientChannel *> map_channel;
			utility::StartableServiceContainer<chaos::NetworkBroker> *network_broker;

			AnswerEngine();
			~AnswerEngine();
		public:
			int registerNewClient(uint32_t client_hash, std::string client_address);
			void sendCacheAnswher(uint32_t client_hash, void *answer);
			
			//! Initialize instance
            void init(void *init_data) throw(chaos::CException);
            
			//! Deinit the implementation
            void deinit() throw(chaos::CException);
		};
		
	}
}

#endif /* defined(__CHAOSFramework__AnswerEngine__) */

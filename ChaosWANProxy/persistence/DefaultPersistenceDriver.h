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
#ifndef __CHAOSFramework__DefaultPersistenceDriver__
#define __CHAOSFramework__DefaultPersistenceDriver__

#include "AbstractPersistenceDriver.h"

#include <vector>
#include <string>

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/network/URLServiceFeeder.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/utility/InizializableService.h>
#include <chaos/common/direct_io/DirectIOClientConnection.h>
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/io/IODirectIODriver.h>
#include <boost/thread.hpp>

namespace chaos {
	namespace wan_proxy {
		namespace persistence {
			//! strucutre to maintains connection information
			typedef struct DirectIOChannelsInfo {
				boost::shared_mutex													connection_mutex;
				chaos::common::direct_io::DirectIOClientConnection					*connection;
				chaos::common::direct_io::channel::DirectIODeviceClientChannel		*device_client_channel;
			} DirectIOChannelsInfo;
			
			//! class that realize the bridget versus cds and metadataserver
			class DefaultPersistenceDriver:
			public AbstractPersistenceDriver,
			protected common::network::URLServiceFeederHandler,
            protected chaos::common::direct_io::DirectIOClientConnectionEventHandler,protected chaos::common::async_central::TimerHandler,
			public chaos::common::utility::InizializableService {
				
				chaos::common::network::NetworkBroker		*network_broker;
				chaos::common::direct_io::DirectIOClient	*direct_io_client;
				chaos::common::message::MDSMessageChannel	*mds_message_channel;
                chaos::common::io::IODataDriverShrdPtr ioLiveDataDriver;

				chaos::common::network::URLServiceFeeder	connection_feeder;
				
                typedef struct _cuids {
                     uint64_t runid;
                     uint64_t pckid;
                     uint64_t last_pckid;
                     uint64_t last_ts;
            } cuids_t;
                cuids_t& getCuid(const std::string& name){return m_cuid[name];}
			protected:
				//!inherited by @common::network::URLServiceFeederHandler
				void  disposeService(void *service_ptr);
				
				//! inherited by @common::network::URLServiceFeederHandler
				void* serviceForURL(const URL& url, uint32_t service_index);
				
				//! inherited by @chaos::common::direct_io::DirectIOClientConnectionEventHandler
				void handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection,
								 chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event);

                void timeout();
                std::map<std::string, cuids_t> m_cuid;

			public:
				DefaultPersistenceDriver(chaos::common::network::NetworkBroker *_network_broker);
				~DefaultPersistenceDriver();
				
				void init(void *init_data) throw (chaos::CException);
				void deinit() throw (chaos::CException);
				
				void addServerList(const std::vector<std::string>& _cds_address_list);
				
				void clear();
				
				//! push a dataset
				int pushNewDataset(const std::string& producer_key,
								   chaos::common::data::CDataWrapper *new_dataset,
								   int store_hint);
				
				//! get a dataset
				int getLastDataset(const std::string& producer_key,
								   chaos::common::data::CDataWrapper **last_dataset);
				
				//! register the dataset of ap roducer
				int registerDataset(const std::string& producer_key,
									chaos::common::data::CDataWrapper& last_dataset);

                void searchMetrics(const std::string&search_string,ChaosStringVector& metrics,bool alive);

                chaos::common::data::CDWShrdPtr searchMetrics(const std::string&search_string,bool alive);
                 /**
                   * query a list of metrics, in the time
                   * \param start start time search
                   * \param end end time search
                   * \param metrics_name metrics to search
                   * \param limit limits search to a number of items
                   * \param res output results
                   * \return zero if success.
                 */
                int queryMetrics(const std::string& start,const std::string& end,const std::vector<std::string>& metrics_name,metrics_results_t& res,int limit=1000);

			};
		}
	}
}

#endif /* defined(__CHAOSFramework__DefaultPersistenceDriver__) */

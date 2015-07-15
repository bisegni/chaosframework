/*
 *	DirectIOClient.cpp
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
#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/utility/InetUtility.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/direct_io/channel/DirectIOVirtualClientChannel.h>
#include <chaos/common/direct_io/DirectIOClientConnectionSharedMetricIO.h>
#include <chaos/common/direct_io/DirectIOClientConnectionMetricCollector.h>

#include <boost/regex.hpp>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

using namespace chaos::common::direct_io;
namespace chaos_data = chaos::common::data;

namespace b_algo = boost::algorithm;

#define DIO_LOG_HEAD "["<<getName()<<"] - "

#define DIOLAPP_ LAPP_ << DIO_LOG_HEAD
#define DIOLDBG_ LDBG_ << DIO_LOG_HEAD << __FUNCTION__
#define DIOLERR_ LERR_ << DIO_LOG_HEAD

DirectIOClient::DirectIOClient(std::string alias):NamedService(alias) {
	
}

DirectIOClient::~DirectIOClient() {
}

void DirectIOClient::forwardEventToClientConnection(DirectIOClientConnection *client,
                                                    DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event_type) {
	client->lowLevelManageEvent(event_type);
}

DirectIOClientConnection *DirectIOClient::getNewConnection(std::string server_description_with_endpoint) {
    uint16_t endpoint;
    std::string server_description;
    DirectIOClientConnection *result = NULL;
    DEBUG_CODE(DIOLDBG_ << "Requested a new connection for a server description " << server_description_with_endpoint;)
    if(decodeServerDescriptionWithEndpoint(server_description_with_endpoint, server_description, endpoint)) {
        DEBUG_CODE(DIOLDBG_ << "scomposed into server description " << server_description << " and endpoint " << endpoint;)
        
        result = _getNewConnectionImpl(server_description,
                                       endpoint);
        
        if(result && GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_DIRECT_IO_LOG_METRIC)) {
            SharedCollectorKey key(server_description, endpoint);
            boost::shared_ptr<DirectIOClientConnectionSharedMetricIO> shared_collector;
            if(map_shared_collectors.count(key)==0) {
                shared_collector.reset(new DirectIOClientConnectionSharedMetricIO(getName(), server_description_with_endpoint));
            } else {
                shared_collector = map_shared_collectors[key];
            }

            //the metric allocator of direct io is a direct subclass of DirectIODispatcher
            result = new DirectIOClientConnectionMetricCollector(server_description,
                                                                 endpoint,
                                                                 shared_collector,
                                                                 result);
        }

    }
        
    return result;
}

//! Release the connection
void DirectIOClient::releaseConnection(DirectIOClientConnection *connection_to_release) {
    if(connection_to_release && GlobalConfiguration::getInstance()->getConfiguration()->getBoolValue(InitOption::OPT_DIRECT_IO_LOG_METRIC)) {
        //the metric allocator of direct io is a direct subclass of DirectIODispatcher
        DirectIOClientConnectionMetricCollector *metric_collector = dynamic_cast<DirectIOClientConnectionMetricCollector*>(connection_to_release);
        if(metric_collector) {
            _releaseConnectionImpl(metric_collector->wrapped_connection);
            delete(metric_collector);
        }
    } else  {
        _releaseConnectionImpl(connection_to_release);
    }
}
/*
 *	ManagedDirectIODataDriver.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 08/03/2017 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/io/ManagedDirectIODataDriver.h>

#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/message/MDSMessageChannel.h>

using namespace chaos;
using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::network;

#define INFO    INFO_LOG(ManagedDirectIODataDriver)
#define DBG     DBG_LOG(ManagedDirectIODataDriver)
#define ERR     ERR_LOG(ManagedDirectIODataDriver)

ManagedDirectIODataDriver::ManagedDirectIODataDriver():
IODirectIODriver("ManagedDirectIODataDriver"),
mds_channel(NULL){}

ManagedDirectIODataDriver::~ManagedDirectIODataDriver() {}

void ManagedDirectIODataDriver::init(void *init_parameter) throw(CException) {
    IODirectIODriver::init(init_parameter);
    mds_channel = NetworkBroker::getInstance()->getMetadataserverMessageChannel();
    if(mds_channel == NULL) {throw CException(-1, "Error creating mds channel", __PRETTY_FUNCTION__);}
    
    //try to fetch best endpoint
    int err = 0;
    data::CDataWrapper *conf = NULL;
    if((err = mds_channel->getDataDriverBestConfiguration(&conf)) || (conf == NULL)) {throw CException(-1, "Error fetching best endpoint from mds channel", __PRETTY_FUNCTION__);}
    
    std::unique_ptr<CDataWrapper> auto_conf(conf);
    updateConfiguration(auto_conf.get());
}


void ManagedDirectIODataDriver::deinit() throw(CException) {
    if(mds_channel) {
        NetworkBroker::getInstance()->disposeMessageChannel(mds_channel);
    }
    IODirectIODriver::deinit();
}

void ManagedDirectIODataDriver::storeLogEntries(const std::string& key,
                                                ChaosStringVector log_entries) throw(CException) {
    int err = 0;
    boost::shared_lock<boost::shared_mutex>(mutext_feeder);
    IODirectIODriverClientChannels	*next_client = static_cast<IODirectIODriverClientChannels*>(connectionFeeder.getService());
    
    if(next_client) {
        if((err = (int)next_client->system_client_channel->pushLogEntries(key, log_entries))) {
            ERR << "Error storing log entries into data service "<<next_client->connection->getServerDescription()<<" with code:" << err;
        }
    } else {
        DEBUG_CODE(DBG << "No available socket->loose log");
    }
}

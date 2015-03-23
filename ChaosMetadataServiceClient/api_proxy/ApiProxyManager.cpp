/*
 *	ApiProxyManager.cpp
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
#include <ChaosMetadataServiceClient/api_proxy/ApiProxyManager.h>

using namespace chaos::metadata_service_client::api_proxy;

    //! default private constructor called only by ChaosMetadataServiceClient class
ApiProxyManager::ApiProxyManager(chaos::common::network::NetworkBroker *_network_broker,
                                 ClientSetting *_setting):
network_broker(_network_broker),
setting(_setting),
mn_message_channel(NULL) {

}

    //! default destructor called only by ChaosMetadataServiceClient class
ApiProxyManager::~ApiProxyManager() {
    
}

void ApiProxyManager::init(void *init_data) throw (chaos::CException) {
    if(!network_broker) throw chaos::CException(-1, "NetworkBroker has not been set", __PRETTY_FUNCTION__);

    mn_message_channel = network_broker->getRawMultiAddressMessageChannel();
    if(!mn_message_channel) throw chaos::CException(-1, "Error multinode message channel creation", __PRETTY_FUNCTION__);
}

void ApiProxyManager::deinit() throw (chaos::CException) {
        //clear all instances
    api_instance.clear();

    if(mn_message_channel) {
        network_broker->disposeMessageChannel(mn_message_channel);
    }
}
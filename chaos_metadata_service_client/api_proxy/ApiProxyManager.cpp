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
#include <chaos_metadata_service_client/api_proxy/ApiProxyManager.h>
#include <chaos_metadata_service_client/ChaosMetadataServiceClient.h>
#include <chaos/common/network/NetworkBroker.h>

#define APM_LAPP INFO_LOG(chaos_metadata_service_client)
#define APM_LDBG DBG_LOG(chaos_metadata_service_client)
#define APM_LERR ERR_LOG(chaos_metadata_service_client)

using namespace chaos::common::network;
using namespace chaos::metadata_service_client;
using namespace chaos::metadata_service_client::api_proxy;

ApiProxyManager::ApiProxyManager():
mn_message_channel(NULL) {}
ApiProxyManager::~ApiProxyManager() {}

void ApiProxyManager::init(void *init_data) throw (chaos::CException) {

    APM_LAPP << "Get multi address message channel";
    mn_message_channel = NetworkBroker::getInstance()->getRawMultiAddressMessageChannel();
    if(!mn_message_channel) throw chaos::CException(-1, "Error multinode message channel creation", __PRETTY_FUNCTION__);
    
    for(BackendServerListIterator it = ChaosMetadataServiceClient::getInstance()->setting.mds_backend_servers.begin();
        it !=  ChaosMetadataServiceClient::getInstance()->setting.mds_backend_servers.end();
        it++) {
        APM_LAPP << "Add " << *it << " metadaa server";
        CNetworkAddress cna(*it);
        mn_message_channel->addNode(cna);
    }
}

void ApiProxyManager::deinit() throw (chaos::CException) {
    if(mn_message_channel) {
        NetworkBroker::getInstance()->disposeMessageChannel(mn_message_channel);
        mn_message_channel = NULL;
    }
}

    //! add a new api server endpoint
void ApiProxyManager::addServerAddress(const std::string& server_address) {
    mn_message_channel->addNode(server_address);
}
    //! remove all api endpoint server
void ApiProxyManager::clearServer() {
    mn_message_channel->removeAllNode();
}

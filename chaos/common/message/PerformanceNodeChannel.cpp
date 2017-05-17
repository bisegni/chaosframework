/*
 *	PerformanceNodeChannel.cpp
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

#include <chaos/common/message/PerformanceNodeChannel.h>
using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::message;
using namespace chaos::common::direct_io;

//! base constructor
PerformanceNodeChannel::PerformanceNodeChannel(NetworkBroker *msg_broker,
                                               CNetworkAddress *_node_network_address,
                                               DirectIOClient *_client_instance,
                                               MessageRequestDomainSHRDPtr _new_message_request_domain):
MessageChannel(msg_broker,
               _new_message_request_domain),
node_network_address(_node_network_address),
client_instance(_client_instance) {}

PerformanceNodeChannel::~PerformanceNodeChannel() {
	releasePerformanceSession(local_performance_session);
}

//Get the performance session for a chaos node
int PerformanceNodeChannel::getPerformanceSession(DirectIOPerformanceSession **performance_session_handler,
												  uint32_t ms_timeout) {
    int err = chaos::ErrorCode::EC_NO_ERROR;
	if(!performance_session_handler) return  -100;
	
	//get the local endpoint
	DirectIOServerEndpoint *local_session_endpoint = getBroker()->getDirectIOServerEndpoint();
	if(!local_session_endpoint) return -101;
	
	std::string remote_endpoint_url;
	CDataWrapper init_performance_session_param;
	
	//set the init parameter
	init_performance_session_param.addStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION,
												  local_session_endpoint->getUrl());
	
	//sent the request and waith the ansewer for startp local session
	ChaosUniquePtr<CDataWrapper> init_session_result(sendRequest(node_network_address->ip_port,
                                                           PerformanceSystemRpcKey::SYSTEM_PERFORMANCE_DOMAIN,
                                                           PerformanceSystemRpcKey::ACTION_PERFORMANCE_INIT_SESSION,
                                                           &init_performance_session_param,
                                                           ms_timeout));
    err = getLastErrorCode();
	if(err == ErrorCode::EC_NO_ERROR) {
        ChaosUniquePtr<CDataWrapper> info_pack(init_session_result->getCSDataValue(RpcActionDefinitionKey::CS_CMDM_ACTION_MESSAGE));
        if(info_pack.get() && info_pack->hasKey(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION)){
			
			remote_endpoint_url = info_pack->getStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION);
            DirectIOClientConnection *local_session_client_connection = client_instance->getNewConnection(remote_endpoint_url);
			if(!local_session_client_connection) {
				//i need to release the enpoint
				getBroker()->releaseDirectIOServerEndpoint(local_session_endpoint);
			}
			
			// i can create session
			local_performance_session = *performance_session_handler = new DirectIOPerformanceSession(local_session_client_connection, local_session_endpoint);
			if(!*performance_session_handler) {
				client_instance->releaseConnection(local_session_client_connection);
				//i need to release the enpoint
				getBroker()->releaseDirectIOServerEndpoint(local_session_endpoint);
				
				err = -103;
			} else {
				try {
					InizializableService::initImplementation(*performance_session_handler, NULL, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
				} catch(chaos::CException ex) {
					InizializableService::deinitImplementation(*performance_session_handler,  "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
					err = -104;
				}
			}
        }
    } else {
		//i need to release the enpoint
		getBroker()->releaseDirectIOServerEndpoint(local_session_endpoint);
		err = -102;
	}
    return err;
}
//! release the performance session
int PerformanceNodeChannel::releasePerformanceSession(DirectIOPerformanceSession *performance_session,
													  uint32_t ms_timeout) {
	CDataWrapper init_performance_session_param;
	if(!performance_session) return -1;
	if(local_performance_session != performance_session) return -1;
	
	try{
		//set the init parameter
		init_performance_session_param.addStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION,
													  performance_session->server_endpoint->getUrl());

		//sent the request and waith the ansewer for startp local session
		ChaosUniquePtr<CDataWrapper> init_session_result(sendRequest(node_network_address->ip_port,
                                                               PerformanceSystemRpcKey::SYSTEM_PERFORMANCE_DOMAIN,
                                                               PerformanceSystemRpcKey::ACTION_PERFORMANCE_CLOSE_SESSION,
                                                               &init_performance_session_param,
                                                               ms_timeout));

		
		InizializableService::deinitImplementation(performance_session,  "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		if(performance_session->client_connection) client_instance->releaseConnection(performance_session->client_connection);
		//i need to release the enpoint
		if(performance_session->server_endpoint) getBroker()->releaseDirectIOServerEndpoint(performance_session->server_endpoint);
		
	} catch(chaos::CException ex) {
		return -100;
	}
	delete(local_performance_session);
	local_performance_session = NULL;
	return 0;
}

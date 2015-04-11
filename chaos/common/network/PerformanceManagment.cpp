/*
 *	PerformanceManagment.cpp
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
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/network/PerformanceManagment.h>
#include <chaos/common/direct_io/DirectIOPerformanceSession.h>

#define PM_LOG_HEAD "[PerformanceManagment] - "

#define PMLAPP_ LAPP_ << PM_LOG_HEAD
#define PMLDBG_ LDBG_ << PM_LOG_HEAD << __FUNCTION__
#define PMLERR_ LERR_ << PM_LOG_HEAD

using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;

PerformanceManagment::PerformanceManagment(NetworkBroker *_network_broker):
network_broker(_network_broker),
global_performance_connection(NULL){
	//create the description for init preformance session rpc action
	AbstActionDescShrPtr
	actionDescription = DeclareAction::addActionDescritionInstance<PerformanceManagment>(this,
																						 &PerformanceManagment::startPerformanceSession,
																						 PerformanceSystemRpcKey::SYSTEM_PERFORMANCE_DOMAIN,
																						 PerformanceSystemRpcKey::ACTION_PERFORMANCE_INIT_SESSION,
																						 "Initialize the performance session");
	actionDescription = DeclareAction::addActionDescritionInstance<PerformanceManagment>(this,
																						 &PerformanceManagment::stopPerformanceSession,
																						 PerformanceSystemRpcKey::SYSTEM_PERFORMANCE_DOMAIN,
																						 PerformanceSystemRpcKey::ACTION_PERFORMANCE_CLOSE_SESSION,
																						 "Deinitialize the performance session");
}
PerformanceManagment::~PerformanceManagment() {
	
}

//init the implementation
void PerformanceManagment::init(void *init_parameter) throw(chaos::CException) {
	//check the network broker setup
	if(!network_broker) throw chaos::CException(-1, "NetworkBroker not set", __PRETTY_FUNCTION__);
	
	//register the action
	network_broker->registerAction(this);
}

//Start the implementation
void PerformanceManagment::start() throw(chaos::CException) {
	PMLAPP_ << "Start the purger thread";
	work_on_purge = true;
	thread_purge.reset(new boost::thread(& PerformanceManagment::purge_worker, this));
}

//Stop the implementation
void PerformanceManagment::stop() throw(chaos::CException) {
	PMLAPP_ << "Stop the purger thread";
	work_on_purge = false;
	purge_wait_semaphore.unlock();
	thread_purge->join();
	PMLAPP_ << "Purger thread stoppped";
	
	PMLAPP_ << "Remove all orfaned performance session";
	purge_map();
}

DirectIOClient *PerformanceManagment::getLocalDirectIOClientInstance() {
	boost::unique_lock<boost::mutex>(mutext_client_connection);
	if(!global_performance_connection) {
		global_performance_connection = network_broker->getDirectIOClientInstance();
		if(!global_performance_connection) throw chaos::CException(-1, "Performance direct io client creation error", __PRETTY_FUNCTION__);
		InizializableService::initImplementation(global_performance_connection, NULL, global_performance_connection->getName(), __PRETTY_FUNCTION__);
	}
	return global_performance_connection;
}

//Deinit the implementation
void PerformanceManagment::deinit() throw(chaos::CException) {
	//register the action
	network_broker->deregisterAction(this);
	
	if(global_performance_connection) {
		InizializableService::deinitImplementation(global_performance_connection, global_performance_connection->getName(), __PRETTY_FUNCTION__);
	}
}

void PerformanceManagment::purge_worker() {
	while(work_on_purge) {
		purge_map();
		purge_wait_semaphore.wait(5000);
	}
}

void PerformanceManagment::purge_map() {
	boost::unique_lock<boost::shared_mutex> lock(mutex_map_purgeable);
	for(std::map<std::string, DirectIOPerformanceSession*>::iterator iter = map_purgeable_performance_node.begin();
		iter != map_purgeable_performance_node.end();) {
		disposePerformanceNode(iter->second);
		map_purgeable_performance_node.erase(iter++);
	}
	
}

void  PerformanceManagment::handleEvent(DirectIOClientConnection *client_connection, DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	if(event !=
	   DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected) return;
	
	//add to purgeable map
	boost::unique_lock<boost::shared_mutex> lock(mutex_map_purgeable);
	if(map_purgeable_performance_node.count(client_connection->getURL())) {
		PMLDBG_ << "Performance session for remote address "<<client_connection->getServerDescription() << " Already in purge map";
		return;
	}
	map_purgeable_performance_node.insert(make_pair(client_connection->getServerDescription(), PMKeyObjectContainer::accessItem(client_connection->getURL())));
	PMLDBG_ << "Performance session for remote address "<<client_connection->getServerDescription() << " added in purge map";
	
}

void  PerformanceManagment::disposePerformanceNode(DirectIOPerformanceSession *performance_node) {
	std::string server_description;
	try {
		server_description = performance_node->client_connection->getURL();
		PMLAPP_ << "Dispose the performance node for "<<server_description;
		InizializableService::initImplementation(performance_node, NULL, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
	}
	catch(CException ex) {}
	catch(...) {}
	
	//deregister the server
	if(performance_node->client_connection) {
		PMLAPP_ << "Deregister the performance node for "<<server_description;
		
		
		PMLAPP_ << "Release client conenction for performance node for "<<server_description;
		global_performance_connection->releaseConnection(performance_node->client_connection);
	}
	if(performance_node->server_endpoint) {
		PMLAPP_ << "Release server endpoint for performance node for "<<server_description;
		network_broker->releaseDirectIOServerEndpoint(performance_node->server_endpoint);
	}
	
	PMKeyObjectContainer::deregisterElementKey(server_description);
}

void  PerformanceManagment::freeObject(std::string server_description, DirectIOPerformanceSession *performance_node) {
	disposePerformanceNode(performance_node);
}

chaos_data::CDataWrapper* PerformanceManagment::startPerformanceSession(chaos_data::CDataWrapper *param, bool& detach) throw(chaos::CException) {
	if(!param) return NULL;
	if(!param->hasKey(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION))
		throw chaos::CException(-1, "Requester server description not found", __PRETTY_FUNCTION__);
	
	chaos_data::CDataWrapper *result = NULL;
	
	//we can initiate performance session allcoation
	std::string req_server_description = param->getStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION);
	
	if(PMKeyObjectContainer::hasKey(req_server_description))
		throw chaos::CException(-2, "performance sesison for requester already allocated", __PRETTY_FUNCTION__);
	
	PMLAPP_<< "Create new performance session for " << req_server_description;
	//ensure lcient creation on local instance
	getLocalDirectIOClientInstance();
	//get the local connection to the requester form shared client
	DirectIOClientConnection *client_connection = global_performance_connection->getNewConnection(req_server_description);
	client_connection->setEventHandler(this);
	
	//get the server endpoint for the requester
	DirectIOServerEndpoint *server_endpoint = network_broker->getDirectIOServerEndpoint();
	
	//alocate new session
	DirectIOPerformanceSession *performace_node = new DirectIOPerformanceSession(client_connection, server_endpoint);
	try {
		
		InizializableService::initImplementation(performace_node, NULL, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		
		//set the result value to the local endpoint url
		result = new chaos_data::CDataWrapper();
		result->addStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION, server_endpoint->getUrl());
		
	} catch(CException ex) {
		InizializableService::deinitImplementation(performace_node, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		if(client_connection) global_performance_connection->releaseConnection(client_connection);
		if(server_endpoint) network_broker->releaseDirectIOServerEndpoint(server_endpoint);
		throw ex;
	} catch(...) {
		InizializableService::deinitImplementation(performace_node, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		if(client_connection) global_performance_connection->releaseConnection(client_connection);
		if(server_endpoint) network_broker->releaseDirectIOServerEndpoint(server_endpoint);
		throw chaos::CException(-3, "Generic exception on initialization of performance loop", __PRETTY_FUNCTION__);
	}
	//register performance node
	PMKeyObjectContainer::registerElement(req_server_description, performace_node);
	
	//get the node server description for send it to the requester
	return result;
}

chaos_data::CDataWrapper* PerformanceManagment::stopPerformanceSession(chaos_data::CDataWrapper *param, bool& detach) throw(chaos::CException) {
	if(!param) return NULL;
	if(!param->hasKey(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION))
		throw chaos::CException(-1, "Requester server description not found", __PRETTY_FUNCTION__);
	//we can initiate performance session allcoation
	std::string req_server_description = param->getStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION);
	
	if(!PMKeyObjectContainer::hasKey(req_server_description))
		throw chaos::CException(-2, "performance sesison for requester already allocated", __PRETTY_FUNCTION__);
	
	//we can proceed to the closing of the performance session
	DirectIOPerformanceSession *performace_node = PMKeyObjectContainer::accessItem(req_server_description);
	
	// dispose the sesison node
	disposePerformanceNode(performace_node);
	return NULL;
}
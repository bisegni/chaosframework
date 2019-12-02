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
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/network/PerformanceManagment.h>
#include <chaos/common/direct_io/DirectIOPerformanceSession.h>
#include <chaos/common/direct_io/DirectIOServerEndpoint.h>
#define PM_LOG_HEAD "[PerformanceManagment] - "

#define PMLAPP_ LAPP_ << PM_LOG_HEAD
#define PMLDBG_ LDBG_ << PM_LOG_HEAD << __FUNCTION__
#define PMLERR_ LERR_ << PM_LOG_HEAD

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::network;
using namespace chaos::common::direct_io;
using namespace chaos::common::async_central;

PerformanceManagment::PerformanceManagment(NetworkBroker *_network_broker):
network_broker(_network_broker),
map_sessions(this),
global_performance_connection(NULL),
work_on_purge(false){
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
void PerformanceManagment::init(void *init_parameter)  {
	//check the network broker setup
	if(!network_broker) throw chaos::CException(-1, "NetworkBroker not set", __PRETTY_FUNCTION__);
	//register the action
	network_broker->registerAction(this);
}

//Start the implementation
void PerformanceManagment::start()  {
	PMLAPP_ << "Start the purger thread";

    AsyncCentralManager::getInstance()->addTimer(this, chaos::common::constants::PerformanceManagerTimersTimeoutinMSec, chaos::common::constants::PerformanceManagerTimersTimeoutinMSec);
}

//Stop the implementation
void PerformanceManagment::stop()  {
	PMLAPP_ << "Stop the purger thread";

    AsyncCentralManager::getInstance()->removeTimer(this);
	PMLAPP_ << "Purger thread stoppped";
	
	PMLAPP_ << "Remove all orfaned performance session";
	purge_map();
}

DirectIOClient *PerformanceManagment::getLocalDirectIOClientInstance() {
	boost::unique_lock<boost::mutex> ul(mutext_client_connection);
	if(!global_performance_connection) {
		global_performance_connection = network_broker->getSharedDirectIOClientInstance();
	}
	return global_performance_connection;
}

//Deinit the implementation
void PerformanceManagment::deinit()  {
	//register the action
	network_broker->deregisterAction(this);
}

void PerformanceManagment::timeout() {
	purge_map();
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
	map_purgeable_performance_node.insert(make_pair(client_connection->getServerDescription(), map_sessions.accessItem(client_connection->getURL())));
	PMLDBG_ << "Performance session for remote address "<<client_connection->getServerDescription() << " added in purge map";
}

void  PerformanceManagment::disposePerformanceNode(DirectIOPerformanceSession *performance_node) {
	std::string server_description;
	try {
		server_description = performance_node->client_connection->getURL();
		PMLAPP_ << "Dispose the performance node for "<<server_description;
		InizializableService::initImplementation(performance_node, NULL, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
	}
	catch(CException& ex) {}
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
	
	map_sessions.deregisterElementKey(server_description);
}

void  PerformanceManagment::freeObject(const PMKeyObjectContainer::TKOCElement& element_to_dispose) {
	disposePerformanceNode(element_to_dispose.element);
}

CDWUniquePtr PerformanceManagment::startPerformanceSession(chaos::common::data::CDWUniquePtr param) {
    CHECK_CDW_THROW_AND_LOG(param, PMLERR_, -1, "No parameter has been set")
    CHECK_KEY_THROW_AND_LOG_FORMATTED(param, PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION, PMLERR_, -2, "No %1% key has been set",%PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION)
    
	CDWUniquePtr result;
	
	//we can initiate performance session allcoation
	std::string req_server_description = param->getStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION);
	
	if(map_sessions.hasKey(req_server_description))
		throw chaos::CException(-2, "performance sesison for requester already allocated", __PRETTY_FUNCTION__);
	
	PMLAPP_<< "Create new performance session for " << req_server_description;
	//ensure lcient creation on local instance
	getLocalDirectIOClientInstance();
	//get the local connection to the requester form shared client
	DirectIOClientConnection *client_connection = global_performance_connection->getNewConnection(req_server_description);
    if(client_connection == NULL) {LOG_AND_TROW(PMLERR_, -3, "Got NULL cleint connection from getNewConnection")}
    
    //set this class as handler
	client_connection->setEventHandler(this);
	
	//get the server endpoint for the requester
	DirectIOServerEndpoint *server_endpoint = network_broker->getDirectIOServerEndpoint();
	
	//alocate new session
	DirectIOPerformanceSession *performace_node = new DirectIOPerformanceSession(client_connection, server_endpoint);
	try {
		
		InizializableService::initImplementation(performace_node, NULL, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		
		//set the result value to the local endpoint url
		result.reset(new CDataWrapper());
		result->addStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION, server_endpoint->getUrl());
		
	} catch(CException& ex) {
		InizializableService::deinitImplementation(performace_node, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		if(client_connection) global_performance_connection->releaseConnection(client_connection);
		if(server_endpoint) network_broker->releaseDirectIOServerEndpoint(server_endpoint);
		throw;
	} catch(...) {
		InizializableService::deinitImplementation(performace_node, "DirectIOPerformanceSession", __PRETTY_FUNCTION__);
		if(client_connection) global_performance_connection->releaseConnection(client_connection);
		if(server_endpoint) network_broker->releaseDirectIOServerEndpoint(server_endpoint);
		throw chaos::CException(-3, "Generic exception on initialization of performance loop", __PRETTY_FUNCTION__);
	}
	//register performance node
	map_sessions.registerElement(req_server_description, performace_node);
	
	//get the node server description for send it to the requester
	return result;
}

CDWUniquePtr PerformanceManagment::stopPerformanceSession(CDWUniquePtr param) {
	if(!param.get()) return CDWUniquePtr();
	if(!param->hasKey(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION))
		throw chaos::CException(-1, "Requester server description not found", __PRETTY_FUNCTION__);
	//we can initiate performance session allcoation
	std::string req_server_description = param->getStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION);
	
	if(!map_sessions.hasKey(req_server_description))
		throw chaos::CException(-2, "performance sesison for requester already allocated", __PRETTY_FUNCTION__);
	
	//we can proceed to the closing of the performance session
	DirectIOPerformanceSession *performace_node = map_sessions.accessItem(req_server_description);
	
	// dispose the sesison node
	disposePerformanceNode(performace_node);
	return CDWUniquePtr();
}

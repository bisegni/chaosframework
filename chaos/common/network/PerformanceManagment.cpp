//
//  PerformanceManagment.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 19/03/14.
//  Copyright (c) 2014 INFN. All rights reserved.
//
#include <chaos/common/network/NetworkBroker.h>
#include <chaos/common/direct_io/DirectIOClient.h>
#include <chaos/common/network/PerformanceManagment.h>
#include <chaos/common/direct_io/DirectIOPerformanceLoop.h>

#define PM_LOG_HEAD "[PerformanceManagment] - "

#define PMLAPP_ LAPP_ << PM_LOG_HEAD
#define PMLDBG_ LDBG_ << PM_LOG_HEAD << __FUNCTION__
#define PMLERR_ LERR_ << PM_LOG_HEAD


using namespace chaos::common::network;
namespace chaos_direct_io = chaos::common::direct_io;

PerformanceManagment::PerformanceManagment(NetworkBroker *_network_broker):
network_broker(_network_broker),
global_performance_connection(NULL),
queue_purgeable_performance_node(1) {
	//create the description for init preformance session rpc action
	AbstActionDescShrPtr
	actionDescription = DeclareAction::addActionDescritionInstance<PerformanceManagment>(this,
																						 &PerformanceManagment::startPerformanceSession,
																						 PerformanceSystemRpcKey::SYSTEM_PERFORMANCE_DOMAIN,
																						 PerformanceSystemRpcKey::ACTION_PERFORMANCE_INIT_SESSION,
																						 "Initialize the performance session");
	

}
PerformanceManagment::~PerformanceManagment() {
	
}

//init the implementation
void PerformanceManagment::init(void *init_parameter) throw(chaos::CException) {
	if(!network_broker) throw chaos::CException(-1, "NetworkBroker not set", __PRETTY_FUNCTION__);
}

//Start the implementation
void PerformanceManagment::start() throw(chaos::CException) {
	PMLAPP_ << "Start the purger thread";
	work_on_purge = true;
	thread_purge.reset(new boost::thread(& PerformanceManagment::purge_worker, this));
}

//Stop the implementation
void PerformanceManagment::stop() throw(chaos::CException) {
	chaos_direct_io::DirectIOPerformanceLoop *performance_node_to_purge = NULL;
	PMLAPP_ << "Stop the purger thread";
	work_on_purge = false;
	purge_wait_semaphore.unlock();
	thread_purge->join();
	PMLAPP_ << "Purger thread stoppped";

	PMLAPP_ << "Remove all orfaned performance session";
	while (!queue_purgeable_performance_node.empty()) {
		if(queue_purgeable_performance_node.pop(performance_node_to_purge)) {
			disposePerformanceNode(performance_node_to_purge);
		} else {
			//no more element
			break;
		}
	}
}

//Deinit the implementation
void PerformanceManagment::deinit() throw(chaos::CException) {
	if(global_performance_connection) {
		chaos::utility::InizializableService::deinitImplementation(global_performance_connection, global_performance_connection->getName(), __PRETTY_FUNCTION__);
	}
}

void PerformanceManagment::purge_worker() {
	chaos_direct_io::DirectIOPerformanceLoop *performance_node_to_purge = NULL;
	while(work_on_purge) {
		for (int idx = 10; idx<=0 && !queue_purgeable_performance_node.empty(); idx--) {
			if(queue_purgeable_performance_node.pop(performance_node_to_purge)) {
				disposePerformanceNode(performance_node_to_purge);
			} else {
				//no more element
			}
		}
		purge_wait_semaphore.wait(5000);
	}
}

void  PerformanceManagment::handleEvent(chaos_direct_io::DirectIOClientConnection *client_connection, chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionStateType event) {
	if(event != chaos_direct_io::DirectIOClientConnectionStateType::DirectIOClientConnectionEventDisconnected) return;
	
	PMLDBG_ << "Managing disconenction for "<<client_connection->getServerDescription();
	
	//add to purgeable map
	queue_purgeable_performance_node.push(TemplatedKeyObjectContainer::accessItem(client_connection->getServerDescription()));
	
	PMLDBG_ << "Performance node added to the purgeable ndoe queue";

}

void  PerformanceManagment::disposePerformanceNode(chaos_direct_io::DirectIOPerformanceLoop *performance_node) {
	std::string server_description;
	try {
		server_description = performance_node->client_connection->getServerDescription();
		PMLAPP_ << "Dispose the performance node for "<<server_description;
		chaos::utility::InizializableService::initImplementation(performance_node, NULL, "DirectIOPerformanceLoop", __PRETTY_FUNCTION__);
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
}

void  PerformanceManagment::freeObject(std::string server_description, chaos_direct_io::DirectIOPerformanceLoop *performance_node) {
	disposePerformanceNode(performance_node);
}

chaos_data::CDataWrapper* PerformanceManagment::startPerformanceSession(chaos_data::CDataWrapper *param, bool& detach) throw(chaos::CException) {
	if(!param) return NULL;
	if(!param->hasKey(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION))
		throw chaos::CException(-1, "Requester server description not found", __PRETTY_FUNCTION__);

	//we can initiate performance session allcoation
	std::string req_server_description = param->getStringValue(PerformanceSystemRpcKey::KEY_REQUEST_SERVER_DESCRITPION);

	if(TemplatedKeyObjectContainer::hasKey(req_server_description))
		throw chaos::CException(-2, "performance sesison for requester already allocated", __PRETTY_FUNCTION__);
	
	PMLAPP_<< "Create new performance session for " << req_server_description;

	//lock the map
	if(!global_performance_connection) {
		global_performance_connection = network_broker->getDirectIOClientInstance();
		if(global_performance_connection) throw chaos::CException(-1, "Performance direct io client creation error", __PRETTY_FUNCTION__);

		chaos::utility::InizializableService::initImplementation(global_performance_connection, NULL, global_performance_connection->getName(), __PRETTY_FUNCTION__);
	}
	chaos_direct_io::DirectIOClientConnection *client_connection = global_performance_connection->getNewConnection(req_server_description);
	chaos_direct_io::DirectIOServerEndpoint *server_endpoint = network_broker->getDirectIOServerEndpoint();
	chaos_direct_io::DirectIOPerformanceLoop *performace_node = new chaos_direct_io::DirectIOPerformanceLoop(client_connection, server_endpoint);
	try {
		
		chaos::utility::InizializableService::initImplementation(performace_node, NULL, "DirectIOPerformanceLoop", __PRETTY_FUNCTION__);

	} catch(CException ex) {
		chaos::utility::InizializableService::deinitImplementation(performace_node, "DirectIOPerformanceLoop", __PRETTY_FUNCTION__);
		if(client_connection) global_performance_connection->releaseConnection(client_connection);
		if(server_endpoint) network_broker->releaseDirectIOServerEndpoint(server_endpoint);
		throw ex;
	} catch(...) {
		chaos::utility::InizializableService::deinitImplementation(performace_node, "DirectIOPerformanceLoop", __PRETTY_FUNCTION__);
		if(client_connection) global_performance_connection->releaseConnection(client_connection);
		if(server_endpoint) network_broker->releaseDirectIOServerEndpoint(server_endpoint);
		throw chaos::CException(-3, "Generic exception on initialization of performance loop", __PRETTY_FUNCTION__);
	}
	//register performance node
	TemplatedKeyObjectContainer::registerElement(req_server_description, performace_node);
	
	//get the node server description for send it to the requester
	return NULL;
}

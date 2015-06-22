/*
 *	ChaosMetadataService.cpp
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
#include "ChaosMetadataService.h"
#include "mds_constants.h"

#include <csignal>
#include <chaos/common/exception/CException.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>


#include <chaos/common/utility/ObjectFactoryRegister.h>

//! Regular expression for check server endpoint with the sintax hostname:[priority_port:service_port]
static const boost::regex KVParamRegex("[a-zA-Z0-9/_-]+:[a-zA-Z0-9/_-]+");

using namespace std;
using namespace chaos;
using namespace chaos::metadata_service;
using namespace chaos::common::utility;
using namespace chaos::metadata_service::api;
using namespace chaos::metadata_service::batch;
using namespace chaos::service_common::persistence::data_access;
using boost::shared_ptr;

WaitSemaphore ChaosMetadataService::waitCloseSemaphore;

#define LCND_LAPP   INFO_LOG(ChaosMetadataService)
#define LCND_LDBG   DBG_LOG(ChaosMetadataService)
#define LCND_LERR   ERR_LOG(ChaosMetadataService)
//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosMetadataService::init(int argc, char* argv[]) throw (CException) {
	ChaosCommon<ChaosMetadataService>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosMetadataService::init(istringstream &initStringStream) throw (CException) {
	ChaosCommon<ChaosMetadataService>::init(initStringStream);
}

/*
 *
 */
void ChaosMetadataService::init(void *init_data)  throw(CException) {
	try {
		ChaosCommon<ChaosMetadataService>::init(init_data);
		if (signal((int) SIGINT, ChaosMetadataService::signalHanlder) == SIG_ERR) {
			throw CException(-1, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
		}
		
		if (signal((int) SIGQUIT, ChaosMetadataService::signalHanlder) == SIG_ERR) {
			throw CException(2, "Error registering SIG_ERR signal", __PRETTY_FUNCTION__);
		}
		
		//scan the setting
		if(!getGlobalConfigurationInstance()->hasOption(OPT_PERSITENCE_IMPL)) {
			//no cache server provided
			throw chaos::CException(-3, "No persistence implementation provided", __PRETTY_FUNCTION__);
		}
		
		if(!getGlobalConfigurationInstance()->hasOption(OPT_PERSITENCE_SERVER_ADDR_LIST)) {
			//no cache server provided
			throw chaos::CException(-4, "No persistence's server list provided", __PRETTY_FUNCTION__);
		}
		
		if(getGlobalConfigurationInstance()->hasOption(OPT_PERSITENCE_KV_PARAMTER)) {
			fillKVParameter(setting.persistence_kv_param_map,
							getGlobalConfigurationInstance()->getOption< std::vector< std::string> >(OPT_PERSITENCE_KV_PARAMTER));
		}
		
        // network broker
		api_subsystem_accessor.network_broker_service.reset(new NetworkBroker(), "NetworkBroker");
		api_subsystem_accessor.network_broker_service.init(NULL, __PRETTY_FUNCTION__);
		
        //! batch system
        api_subsystem_accessor.batch_executor.reset(new MDSBatchExecutor("MDSBatchExecutor",
                                                                         api_subsystem_accessor.network_broker_service.get()),
                                                    "MDSBatchExecutor");
        api_subsystem_accessor.batch_executor.init(NULL, __PRETTY_FUNCTION__);
        
		// persistence driver system
		const std::string persistence_driver_name = setting.persistence_implementation + "PersistenceDriver";
		AbstractPersistenceDriver *instance = ObjectFactoryRegister<AbstractPersistenceDriver>::getInstance()->getNewInstanceByName(persistence_driver_name);
		if(!instance) throw chaos::CException(-5, "No persistence driver instance found", __PRETTY_FUNCTION__);
		api_subsystem_accessor.persistence_driver.reset(instance, "AbstractPersistenceDriver");
		api_subsystem_accessor.persistence_driver.init((void*)&setting, __PRETTY_FUNCTION__);
        
        //api system
        api_managment_service.reset(new ApiManagment(), "ApiManagment");
        api_managment_service.init(static_cast<void*>(&api_subsystem_accessor), __PRETTY_FUNCTION__);

        //connect persistence driver to batch system
        api_subsystem_accessor.batch_executor->abstract_persistance_driver = api_subsystem_accessor.persistence_driver.get();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
		exit(1);
	}
	//start data manager
}

/*
 *
 */
void ChaosMetadataService::start()  throw(CException) {
	//lock o monitor for waith the end
	try {
		//start network brocker
		api_subsystem_accessor.network_broker_service.start(__PRETTY_FUNCTION__);
        
        //start batch system
        api_subsystem_accessor.batch_executor.start(__PRETTY_FUNCTION__);

        LAPP_ << "-----------------------------------------";
        LAPP_ << "!CHAOS Metadata service started";
        LAPP_ << "RPC Server address: "	<< api_subsystem_accessor.network_broker_service->getRPCUrl();
        LAPP_ << "DirectIO Server address: " << api_subsystem_accessor.network_broker_service->getDirectIOUrl();
        LAPP_ << "Sync RPC URL: "	<< api_subsystem_accessor.network_broker_service->getSyncRPCUrl();
        LAPP_ << "-----------------------------------------";
		//at this point i must with for end signal
		waitCloseSemaphore.wait();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	}
	//execute the deinitialization of CU
	try{
		stop();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	}
	
	try{
		deinit();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	}
}

/*
 Stop the toolkit execution
 */
void ChaosMetadataService::stop()   throw(CException) {
    //stop batch system
    api_subsystem_accessor.batch_executor.stop(__PRETTY_FUNCTION__);
    
	//stop network broker
	api_subsystem_accessor.network_broker_service.stop(__PRETTY_FUNCTION__);
	
	//endWaithCondition.notify_one();
	waitCloseSemaphore.unlock();
}

/*
 Deiniti all the manager
 */
void ChaosMetadataService::deinit()   throw(CException) {
    
    //deinit api system
    CHAOS_NOT_THROW(api_managment_service.deinit(__PRETTY_FUNCTION__);)
	
    //deinit batch system
    CHAOS_NOT_THROW(api_subsystem_accessor.batch_executor.deinit(__PRETTY_FUNCTION__);)
    
    //deinit persistence driver system
	CHAOS_NOT_THROW(api_subsystem_accessor.persistence_driver.deinit(__PRETTY_FUNCTION__);)
	
	//deinit network broker
	CHAOS_NOT_THROW(api_subsystem_accessor.network_broker_service.deinit(__PRETTY_FUNCTION__);)
	LAPP_ << "-----------------------------------------";
	LAPP_ << "Metadata service has been stopped";
	LAPP_ << "-----------------------------------------";
}

/*
 *
 */
void ChaosMetadataService::signalHanlder(int signalNumber) {
	//lock lk(monitor);
	//unlock the condition for end start method
	//endWaithCondition.notify_one();
	waitCloseSemaphore.unlock();
}

void ChaosMetadataService::fillKVParameter(std::map<std::string, std::string>& kvmap,
										   const std::vector<std::string>& multitoken_param) {
	
	std::vector<std::string> kv_splitted;
	std::vector<std::string> kvtokens;
	for(std::vector<std::string>::const_iterator it = multitoken_param.begin();
		it != multitoken_param.end();
		it++) {
		
		const std::string& param_key = *it;
		
		if(!regex_match(param_key, KVParamRegex)) {
			throw chaos::CException(-3, "Malformed kv parameter string", __PRETTY_FUNCTION__);
		}

		
		boost::algorithm::split(kvtokens,
								param_key,
								boost::algorithm::is_any_of("-"),
								boost::algorithm::token_compress_on);
		
		//clear previosly pair
		kv_splitted.clear();
		
		//get new pair
		boost::algorithm::split(kv_splitted,
								param_key,
								boost::algorithm::is_any_of(":"),
								boost::algorithm::token_compress_on);
		// add key/value pair
		kvmap.insert(make_pair(kv_splitted[0], kv_splitted[1]));
	}
}
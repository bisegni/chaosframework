/*
 *	ChaosDataService.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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


#include <csignal>

#include "ChaosDataService.h"
#include "DriverPoolManager.h"

#include <boost/format.hpp>
#include <chaos/common//direct_io/DirectIOServerEndpoint.h>

using namespace std;
using namespace chaos;
using namespace chaos::common::utility;
using namespace chaos::data_service;
using boost::shared_ptr;

//! Regular expression fro token parameter check
static const boost::regex KVParamRegex("[a-zA-Z0-9/_-]+:.+");

#define ChaosDataService_LOG_HEAD "[ChaosDataService] - "

#define CDSLAPP_ LAPP_ << ChaosDataService_LOG_HEAD
#define CDSLDBG_ LDBG_ << ChaosDataService_LOG_HEAD
#define CDSLERR_ LERR_ << ChaosDataService_LOG_HEAD

//boost::mutex ChaosCUToolkit::monitor;
//boost::condition ChaosCUToolkit::endWaithCondition;
WaitSemaphore ChaosDataService::waitCloseSemaphore;

ChaosDataService::ChaosDataService()  {
	
}

ChaosDataService::~ChaosDataService() {
	
}

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosDataService::init(int argc, char* argv[]) throw (CException) {
	ChaosCommon<ChaosDataService>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosDataService::init(istringstream &initStringStream) throw (CException) {
	ChaosCommon<ChaosDataService>::init(initStringStream);
}

void ChaosDataService::fillKVParameter(std::map<std::string, std::string>& kvmap,
									   const std::vector<std::string>& multitoken_parameter) {
	//no cache server provided
	//clear previosly pair
	std::vector<std::string> kv_splitted;
	for(std::vector<std::string>::const_iterator it = multitoken_parameter.begin();
		it != multitoken_parameter.end();
		it++) {
		
		const std::string& kv_param_value = *it;
		
		if(!regex_match(kv_param_value, KVParamRegex)) {
			throw chaos::CException(-3, "Malformed kv parameter string", __PRETTY_FUNCTION__);
		}
		
		//clear previosly pair
		kv_splitted.clear();
		
		//get new pair
		boost::algorithm::split(kv_splitted,
								kv_param_value,
								boost::algorithm::is_any_of(":"),
								boost::algorithm::token_compress_on);
		// add key/value pair
		kvmap.insert(make_pair(kv_splitted[0], kv_splitted[1]));
	}
	
}

/*
 *
 */
void ChaosDataService::init(void *init_data)  throw(CException) {
	try {
		ChaosCommon<ChaosDataService>::init(init_data);
		CDSLAPP_ << "Initializing CHAOS Control System Library";
		if (signal((int) SIGINT, ChaosDataService::signalHanlder) == SIG_ERR) {
			CDSLERR_ << "SIGINT Signal handler registraiton error";
		}
		
		if (signal((int) SIGQUIT, ChaosDataService::signalHanlder) == SIG_ERR) {
			CDSLERR_ << "SIGQUIT Signal handler registraiton error";
		}
		
		if (signal((int) SIGTERM, ChaosDataService::signalHanlder) == SIG_ERR) {
			CDSLERR_ << "SIGTERM Signal handler registraiton error";
		}
		
		//check for mandatory configuration
		if(!getGlobalConfigurationInstance()->hasOption(OPT_RUN_MODE)) {
			//no cache server provided
			throw chaos::CException(-1, "No run mode specified", __PRETTY_FUNCTION__);
		}
		
		if(!setting.cache_only &&																		//we aren't in cache only
		   (getGlobalConfigurationInstance()->getOption<unsigned int>(OPT_RUN_MODE) > BOTH ||	//check if we have a valid run mode
			getGlobalConfigurationInstance()->getOption<unsigned int>(OPT_RUN_MODE) < QUERY)) {
			   //no cache server provided
			   throw chaos::CException(-2, "Invalid run mode", __PRETTY_FUNCTION__);
		   }
		
		//get the run mode and if we are in cache only.... enable only query mode
		run_mode = setting.cache_only?QUERY:(RunMode)getGlobalConfigurationInstance()->getOption<unsigned int>(OPT_RUN_MODE);
		
		//check for mandatory configuration
		if(!getGlobalConfigurationInstance()->hasOption(OPT_CACHE_SERVER_LIST)) {
			//no cache server provided
			throw chaos::CException(-3, "No cache server provided", __PRETTY_FUNCTION__);
		}
		if(run_mode == !getGlobalConfigurationInstance()->hasOption(OPT_DB_DRIVER_SERVERS)) {
			//no cache server provided
			throw chaos::CException(-4, "No index server provided", __PRETTY_FUNCTION__);
		}
		
		if(getGlobalConfigurationInstance()->hasOption(OPT_CACHE_DRIVER_KVP)) {
			fillKVParameter(setting.cache_driver_setting.key_value_custom_param,
							getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_CACHE_DRIVER_KVP));
		}
		
		if(!setting.cache_only && getGlobalConfigurationInstance()->hasOption(OPT_VFS_STORAGE_DRIVER_KVP)) {
			fillKVParameter(setting.file_manager_setting.storage_driver_setting.key_value_custom_param,
							getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_VFS_STORAGE_DRIVER_KVP));
		}
		
		if(!setting.cache_only && getGlobalConfigurationInstance()->hasOption(OPT_DB_DRIVER_KVP)) {
			fillKVParameter(setting.db_driver_setting.key_value_custom_param,
							getGlobalConfigurationInstance()->getOption< std::vector<std::string> >(OPT_DB_DRIVER_KVP));
		}
		
		//allocate the db driver
		if(!setting.cache_only && setting.db_driver_impl.compare("")) {
			//we have a db driver setuped
			std::string db_driver_class_name = boost::str(boost::format("%1%DBDriver") % setting.db_driver_impl);
			CDSLAPP_ << "Allocate index driver of type "<<db_driver_class_name;
			db_driver_ptr = ObjectFactoryRegister<db_system::DBDriver>::getInstance()->getNewInstanceByName(db_driver_class_name);
			if(!db_driver_ptr) throw chaos::CException(-6, "No index driver found", __PRETTY_FUNCTION__);
			InizializableService::initImplementation(db_driver_ptr, &setting.db_driver_setting, db_driver_ptr->getName(), __PRETTY_FUNCTION__);
		}
		
        //initilize driver pool manager
        InizializableService::initImplementation(DriverPoolManager::getInstance(), NULL, "DriverPoolManager", __PRETTY_FUNCTION__);
        
		//check if we are in cache only
		if(!setting.cache_only) {
			//configure the domain url equal to the directio io server one plus the deafult endpoint "0"
			setting.file_manager_setting.storage_driver_setting.domain.local_url = NetworkBroker::getInstance()->getDirectIOUrl();
			setting.file_manager_setting.storage_driver_setting.domain.local_url.append("|0");
			
			//initialize vfs file manager
			CDSLAPP_ << "Allocate VFS Manager";
			vfs_file_manager.reset(new vfs::VFSManager(db_driver_ptr), "VFSFileManager");
			vfs_file_manager.init(&setting.file_manager_setting, __PRETTY_FUNCTION__);
			
		}
		
		if(run_mode == QUERY ||
		   run_mode == BOTH) {
			CDSLAPP_ << "Allocate the Query Data Consumer";
			data_consumer.reset(new QueryDataConsumer(vfs_file_manager.get(), db_driver_ptr), "QueryDataConsumer");
			if(!data_consumer.get()) throw chaos::CException(-7, "Error instantiating data consumer", __PRETTY_FUNCTION__);
			data_consumer->network_broker = NetworkBroker::getInstance();
			data_consumer.init(NULL, __PRETTY_FUNCTION__);
		}
		if(run_mode == INDEXER ||
		   run_mode == BOTH) {
			CDSLAPP_ << "Allocate the Data Consumer";
			stage_data_consumer.reset(new StageDataConsumer(vfs_file_manager.get(), db_driver_ptr, &setting), "StageDataConsumer");
			if(!stage_data_consumer.get()) throw chaos::CException(-8, "Error instantiating stage data consumer", __PRETTY_FUNCTION__);
			stage_data_consumer.init(NULL, __PRETTY_FUNCTION__);
		}
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
		exit(1);
	}
}

/*
 *
 */
void ChaosDataService::start() throw(CException) {
	try {
        ChaosCommon<ChaosDataService>::start();
        
		if(run_mode == QUERY ||
		   run_mode == BOTH) {
			data_consumer.start( __PRETTY_FUNCTION__);
		}
		if(run_mode == INDEXER ||
		   run_mode == BOTH) {
			stage_data_consumer.start(__PRETTY_FUNCTION__);
		}
		
		//print information header on CDS address
		CDSLAPP_ << "--------------------------------------------------------------------------------------";
		CDSLAPP_ << "Chaos Data Service published with url: " << NetworkBroker::getInstance()->getDirectIOUrl() << "|0";
		CDSLAPP_ << "--------------------------------------------------------------------------------------";
		
		waitCloseSemaphore.wait();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
	}
	
	try {
		CDSLAPP_ << "Stoppping CHAOS Data Service";
		stop();
		
		deinit();
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
		exit(1);
	}
}

/*
 Stop the toolkit execution
 */
void ChaosDataService::stop() throw(CException) {
	
	if(run_mode == QUERY ||
	   run_mode == BOTH) {
		data_consumer.stop( __PRETTY_FUNCTION__);
	}
	if(run_mode == INDEXER ||
	   run_mode == BOTH) {
		stage_data_consumer.stop(__PRETTY_FUNCTION__);
	}
    
    ChaosCommon<ChaosDataService>::stop();
}

/*
 Deiniti all the manager
 */
void ChaosDataService::deinit() throw(CException) {
	
	if((run_mode == QUERY ||
		run_mode == BOTH ) &&
	   data_consumer.get()) {
		data_consumer.deinit(__PRETTY_FUNCTION__);
		CDSLAPP_ << "Release the endpoint associated to the Data Consumer";
        NetworkBroker::getInstance()->releaseDirectIOServerEndpoint(data_consumer->server_endpoint);
	}
	if((run_mode == INDEXER ||
		run_mode == BOTH ) &&
	   stage_data_consumer.get()) {
		stage_data_consumer.deinit(__PRETTY_FUNCTION__);
	}
	
	//deinitialize vfs file manager
	vfs_file_manager.deinit(__PRETTY_FUNCTION__);
	
	if(db_driver_ptr) {
		CDSLAPP_ << "Deallocate index driver";
		try {
			InizializableService::deinitImplementation(db_driver_ptr, db_driver_ptr->getName(), __PRETTY_FUNCTION__);
		} catch (chaos::CException e) {
			CDSLAPP_ << e.what();
		}
		delete (db_driver_ptr);
	}
	
    //deinitilize driver pool manager
    InizializableService::deinitImplementation(DriverPoolManager::getInstance(), "DriverPoolManager", __PRETTY_FUNCTION__);
    
	ChaosCommon<ChaosDataService>::deinit();
	
	CDSLAPP_ << "Chaos Data service will exit now";
}


/*
 *
 */
void ChaosDataService::signalHanlder(int signalNumber) {
	waitCloseSemaphore.unlock();
}

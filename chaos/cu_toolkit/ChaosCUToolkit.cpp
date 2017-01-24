/*
 *	ChaosCUToolkit.cpp
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
#include <chaos/cu_toolkit/ChaosCUToolkit.h>
#include <chaos/cu_toolkit/data_manager/DataManager.h>
#include <chaos/cu_toolkit/command_manager/CommandManager.h>

#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/common/metadata_logging/MetadataLoggingManager.h>

#include <csignal>


using namespace std;
using namespace chaos::common::utility;

using namespace chaos::cu;
using namespace chaos::common::healt_system;
using namespace chaos::cu::data_manager;
using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;
using namespace chaos::cu::driver_manager;
using namespace chaos::common::metadata_logging;

//boost::mutex ChaosCUToolkit::monitor;
//boost::condition ChaosCUToolkit::endWaithCondition;
chaos::WaitSemaphore ChaosCUToolkit::waitCloseSemaphore;

ChaosCUToolkit::ChaosCUToolkit() {
    
GlobalConfiguration::getInstance()->addOption<bool>(CU_OPT_IN_MEMORY_DATABASE,
                                                        "Specify when to use in memory or on disc contorl unit internal database",
                                                        true);
    //
    
    
    GlobalConfiguration::getInstance()->addOption<bool>(CONTROL_MANAGER_UNIT_SERVER_ENABLE,
                                                        CONTROL_MANAGER_UNIT_SERVER_ENABLE_desc,
                                                        true);
    
    GlobalConfiguration::getInstance()->addOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS,
                                                               CONTROL_MANAGER_UNIT_SERVER_ALIAS_desc);
    
    GlobalConfiguration::getInstance()->addOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_KEY,
                                                               CONTROL_MANAGER_UNIT_SERVER_KEY_desc);
    
    
    GlobalConfiguration::getInstance()->addOption<uint64_t>(CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC,
                                                            CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_desc,
                                                            CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC_DEFAULT);
    
    GlobalConfiguration::getInstance()->addOption< std::vector<std::string> >(CONTROL_MANAGER_EXECUTION_POOLS,
                                                                              CONTROL_MANAGER_EXECUTION_POOLS_DESC);
    
    GlobalConfiguration::getInstance()->addOption< double >(CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP,
                                                            CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DESC,
                                                            CONTROL_MANAGER_EXECUTION_POOLS_CPU_CAP_DEFAULT);
    
    
}

ChaosCUToolkit::~ChaosCUToolkit() {
    
}

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosCUToolkit::init(int argc, char* argv[]) throw (CException) {
    ChaosCommon<ChaosCUToolkit>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosCUToolkit::init(istringstream &initStringStream) throw (CException) {
    ChaosCommon<ChaosCUToolkit>::init(initStringStream);
}

/*
 *
 */
void ChaosCUToolkit::init(void *init_data)  throw(CException) {
    try {
        ChaosCommon<ChaosCUToolkit>::init(init_data);
        LAPP_ << "Initializing !CHAOS Control Unit System";
        if (signal((int) SIGINT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGINT Signal handler registration error";
        }
        
        if (signal((int) SIGQUIT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGQUIT Signal handler registration error";
        }
        
        if (signal((int) SIGTERM, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGTERM Signal handler registration error";
        }

        if (signal((int) SIGSEGV, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGSEGV Signal handler registration error";
        }

        if (signal((int) SIGABRT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGABRT Signal handler registration error";
        }
        
        //force first allocation of metadata logging
        if(GlobalConfiguration::getInstance()->getMetadataServerAddressList().size()) {
            //we can initilize the logging manager
            InizializableService::initImplementation(chaos::common::metadata_logging::MetadataLoggingManager::getInstance(), NULL, "MetadataLoggingManager", __PRETTY_FUNCTION__);
        }
        
        
        //init healt manager singleton
        StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealtManager", __PRETTY_FUNCTION__);
        
        StartableService::initImplementation(CommandManager::getInstance(), NULL, "CommandManager", "ChaosCUToolkit::init");
        CommandManager::getInstance()->server_handler=this;
        
        StartableService::initImplementation(DriverManager::getInstance(), NULL, "DriverManager", "ChaosCUToolkit::init");
        
        StartableService::initImplementation(DataManager::getInstance(), NULL, "DataManager", "ChaosCUToolkit::init");
        
        StartableService::initImplementation(ControlManager::getInstance(), NULL, "ControlManager", "ChaosCUToolkit::init");
        
        LAPP_ << "Control Manager Initialized";
        
        LAPP_ << "!CHAOS Control Unit System Initialized";
        
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    //start data manager
}

/*
 *
 */
void ChaosCUToolkit::start() throw(CException){
    try {
        ChaosCommon<ChaosCUToolkit>::start();
        
        LAPP_ << "Starting !CHAOS Control Unit System";
        
        //init healt manager singleton
        StartableService::startImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__);
        
        //start command manager, this manager must be the last to startup
        StartableService::startImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::start");
        
        //start driver manager
        StartableService::startImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::start");
        
        //start command manager, this manager must be the last to startup
        StartableService::startImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::start");
        
        //start Control Manager
        StartableService::startImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::start");
        
        LAPP_ << "-----------------------------------------";
        LAPP_ << "!CHAOS Control Unit System Started";
        LAPP_ << "RPC Server address: " << CommandManager::getInstance()->broker->getRPCUrl();
        LAPP_ << "DirectIO Server address: " << CommandManager::getInstance()->broker->getDirectIOUrl();
        LAPP_ << "-----------------------------------------";
        //at this point i must with for end signal
        waitCloseSemaphore.wait();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    //execute the stop and the deinitialization of the toolkit
    stop();
    deinit();
}

/*
 Stop the toolkit execution
 */
void ChaosCUToolkit::stop() throw(CException) {
    //stop command manager, this manager must be the last to startup
    CHAOS_NOT_THROW(StartableService::stopImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop"););
    
    //stop control manager
    CHAOS_NOT_THROW(StartableService::stopImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop"););
    
    //stop command manager, this manager must be the last to startup
    CHAOS_NOT_THROW(StartableService::stopImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::stop"););
    
    //stop driver manager
    CHAOS_NOT_THROW(StartableService::stopImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::stop"););
    
    //init healt manager singleton
    CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
    
    ChaosCommon<ChaosCUToolkit>::stop();
}

/*
 Deiniti all the manager
 */
void ChaosCUToolkit::deinit() throw(CException) {
    LAPP_ << "Stopping !CHAOS Control Unit System";
    //deinit command manager, this manager must be the last to startup
    CHAOS_NOT_THROW(StartableService::deinitImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop"););
    
    //deinit control manager
    CHAOS_NOT_THROW(StartableService::deinitImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop"););
    
    //deinit data manager
    CHAOS_NOT_THROW(StartableService::deinitImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::deinit"););
    
    //deinit metadata logging manager
    CHAOS_NOT_THROW(StartableService::deinitImplementation(DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::deinit"););
    
    CHAOS_NOT_THROW(InizializableService::deinitImplementation(MetadataLoggingManager::getInstance(), "MetadataLoggingManager", __PRETTY_FUNCTION__););
    LAPP_ << "!CHAOS Control Unit System Stopped";
    
    //deinit healt manager singleton
    CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
    
    
    //forward the deinitialization to the common sublayer
    ChaosCommon<ChaosCUToolkit>::deinit();
    
    LAPP_ << "-----------------------------------------";
    LAPP_ << "!CHAOS Control Unit System deinitialized  ";
    LAPP_ << "-----------------------------------------";
}

/*
 * Add a new Control Unit Class for execution
 */
boost::shared_ptr<ControlUnitApiInterface>  ChaosCUToolkit::addProxyControlUnit(const std::string& unique_id) {
    //call command manager to submit Custom Control Unit
    return ControlManager::getInstance()->createNewProxyControlUnit(unique_id);
}

/*
 *
 */
void ChaosCUToolkit::signalHanlder(int signalNumber) {
    //lock lk(monitor);
    //unlock the condition for end start method
    //endWaithCondition.notify_one();
    //waitCloseSemaphore.unlock();
	if((signalNumber==SIGABRT) || (signalNumber==SIGSEGV)){
		LAPP_ << "INTERNAL ERROR, please provide log, Catch SIGNAL: "<< signalNumber;

	    throw CFatalException(signalNumber,"trapped signal",__PRETTY_FUNCTION__);
	 } else {
			LAPP_ << "CATCH SIGNAL "<< signalNumber;
			if(signalNumber == SIGINT || signalNumber == SIGTERM){
				LAPP_ << "EXITING THROUGH SIGNAL '"<<signalNumber<<"' ....";

				 exit(0);
			}

	 }

}

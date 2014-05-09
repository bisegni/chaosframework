/*	
 *	ChaosCUToolkit.cpp
 *	!CHOAS
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
#include <chaos/cu_toolkit/DataManager/DataManager.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <chaos/cu_toolkit/ControlManager/ControlManager.h>

#include <csignal>


using namespace std;
using namespace chaos;
using namespace chaos::cu;

using boost::shared_ptr;

namespace cu_driver_manager = chaos::cu::driver_manager;
namespace common_utility = chaos::utility;

//boost::mutex ChaosCUToolkit::monitor;
//boost::condition ChaosCUToolkit::endWaithCondition;
WaitSemaphore ChaosCUToolkit::waitCloseSemaphore;

ChaosCUToolkit::ChaosCUToolkit() {
	GlobalConfiguration::getInstance()->addOption<bool>(CU_OPT_IN_MEMORY_DATABASE,
														"Specify when to use in memory or on disc contorl unit internal database",
														true);
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
        
        LAPP_ << "Initializing CHAOS Control System Library";
        ChaosCommon<ChaosCUToolkit>::init(init_data);
        if (signal((int) SIGINT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGINT Signal handler registraiton error";
        }
 
        if (signal((int) SIGQUIT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGQUIT Signal handler registraiton error";
        }
    

		chaos::utility::StartableService::initImplementation(cu_driver_manager::DriverManager::getInstance(), NULL, "DriverManager", "ChaosCUToolkit::init");

		chaos::utility::StartableService::initImplementation(DataManager::getInstance(), NULL, "DataManager", "ChaosCUToolkit::init");
        
            //start command manager, this manager must be the last to startup
        chaos::utility::StartableService::initImplementation(CommandManager::getInstance(), NULL, "CommandManager", "ChaosCUToolkit::init");
        CommandManager::getInstance()->privLibControllerPtr=this;
        
            //start Control Manager
		chaos::utility::StartableService::initImplementation(ControlManager::getInstance(), NULL, "ControlManager", "ChaosCUToolkit::init");

        
        LAPP_ << "Control Manager Initialized";
        
        LAPP_ << "CHAOS Control System Library Initialized";

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
        LAPP_ << "Starting CHAOS Control System Library";
		
		chaos::utility::StartableService::startImplementation(cu_driver_manager::DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::start");
		
            //start command manager, this manager must be the last to startup
        chaos::utility::StartableService::startImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::start");
        
            //start command manager, this manager must be the last to startup
		chaos::utility::StartableService::startImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::start");
        
            //start Control Manager
		chaos::utility::StartableService::startImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::start");

        LAPP_ << "-----------------------------------------";
        LAPP_ << "CHAOS Control System Library Started";
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
	chaos::utility::StartableService::stopImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::stop");

	//stop command manager, this manager must be the last to startup
    chaos::utility::StartableService::stopImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::stop");

	//start command manager, this manager must be the last to startup
	chaos::utility::StartableService::stopImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::stop");
    
	//stop driver manager
	chaos::utility::StartableService::stopImplementation(cu_driver_manager::DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::stop");

    waitCloseSemaphore.unlock();
}

/*
 Deiniti all the manager
 */
void ChaosCUToolkit::deinit() throw(CException) {
    LAPP_ << "Stopping CHAOS Control System Library";
        //start Control Manager
    LAPP_ << "Stopping Control Manager";
    chaos::utility::StartableService::deinitImplementation(ControlManager::getInstance(), "ControlManager", "ChaosCUToolkit::deinit");
    LAPP_ << "Control Manager Stopped";
    
        //start command manager, this manager must be the last to startup
    chaos::utility::StartableService::deinitImplementation(CommandManager::getInstance(), "CommandManager", "ChaosCUToolkit::deinit");
    
        //start data manager
	chaos::utility::StartableService::deinitImplementation(DataManager::getInstance(), "DataManager", "ChaosCUToolkit::deinit");
    
	chaos::utility::StartableService::deinitImplementation(cu_driver_manager::DriverManager::getInstance(), "DriverManager", "ChaosCUToolkit::deinit");
    LAPP_ << "CHAOS Control System Library Stopped";
	
	//forward the deinitialization to the common sublayer
	ChaosCommon<ChaosCUToolkit>::deinit();
}

/*
 * Add a new Control Unit Class for execution
 */
void ChaosCUToolkit::addControlUnit(AbstractControlUnit *newCU) {
    //call command manager to submit Custom Control Unit
    ControlManager::getInstance()->submitControlUnit(newCU);
    
    LAPP_ << "A new Control Unit " << newCU->getCUInstance() << " has been submitted";
}

/*
 *
 */
void ChaosCUToolkit::signalHanlder(int signalNumber) {
    //lock lk(monitor);
        //unlock the condition for end start method
    //endWaithCondition.notify_one();
     waitCloseSemaphore.unlock();
}

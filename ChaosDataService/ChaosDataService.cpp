/*
 *	ChaosDataService.cpp
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


#include <csignal>

#include "ChaosDataService.h"

using namespace std;
using namespace chaos;
using namespace chaos::data_service;
using boost::shared_ptr;


//boost::mutex ChaosCUToolkit::monitor;
//boost::condition ChaosCUToolkit::endWaithCondition;
WaitSemaphore ChaosDataService::waitCloseSemaphore;


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

/*
 *
 */
void ChaosDataService::init(void *init_data)  throw(CException) {
    try {
        LAPP_ << "Initializing CHAOS Control System Library";
        ChaosCommon<ChaosDataService>::init(init_data);
        if (signal((int) SIGINT, ChaosDataService::signalHanlder) == SIG_ERR){
            LERR_ << "SIGINT Signal handler registraiton error";
        }
        
        if (signal((int) SIGQUIT, ChaosDataService::signalHanlder) == SIG_ERR){
            LERR_ << "SIGQUIT Signal handler registraiton error";
        }

		LAPP_ << "Allocate Network Brocker";
        network_broker = new utility::StartableServiceContainer<chaos::NetworkBroker>(true);
		network_broker->init(NULL, __FUNCTION__);
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    //start data manager
}

/*
 *
 */
void ChaosDataService::start() throw(CException) {
    try {
        LAPP_ << "Starting CHAOS Data Service";
		network_broker->start(__FUNCTION__);
		
        waitCloseSemaphore.wait();
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    }
    
    try {
        LAPP_ << "Stoppping CHAOS Data Service";
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
	LAPP_ << "Stopping CHAOS Data Service";
    network_broker->stop(__FUNCTION__);
}

/*
 Deiniti all the manager
 */
void ChaosDataService::deinit() throw(CException) {
    LAPP_ << "Deinitializing CHAOS Data Service";
	network_broker->deinit(__FUNCTION__);
	delete(network_broker);
}


/*
 *
 */
void ChaosDataService::signalHanlder(int signalNumber) {
    waitCloseSemaphore.unlock();
}

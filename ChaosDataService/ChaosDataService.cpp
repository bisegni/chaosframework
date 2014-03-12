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
#include <chaos/common//direct_io/DirectIOServerEndpoint.h>
using namespace std;
using namespace chaos;
using namespace chaos::data_service;
using boost::shared_ptr;


#define ChaosDataService_LOG_HEAD "[ChaosDataService] - "

#define CDSLAPP_ LAPP_ << ChaosDataService_LOG_HEAD
#define CDSLDBG_ LDBG_ << ChaosDataService_LOG_HEAD
#define CDSLERR_ LERR_ << ChaosDataService_LOG_HEAD
#define __METHOD_NAME__(x) x(__PRETTY_FUNCTION__)
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

/*
 *
 */
void ChaosDataService::init(void *init_data)  throw(CException) {
    try {
        CDSLAPP_ << "Initializing CHAOS Control System Library";
        ChaosCommon<ChaosDataService>::init(init_data);
        if (signal((int) SIGINT, ChaosDataService::signalHanlder) == SIG_ERR){
            CDSLERR_ << "SIGINT Signal handler registraiton error";
        }
        
        if (signal((int) SIGQUIT, ChaosDataService::signalHanlder) == SIG_ERR){
            CDSLERR_ << "SIGQUIT Signal handler registraiton error";
        }

		CDSLAPP_ << "Allocate Network Brocker";
        network_broker.reset(new NetworkBroker(), "NetworkBroker");
		if(!network_broker.get()) throw chaos::CException(-1, "Error instantiating network broker", __PRETTY_FUNCTION__);
		network_broker.init(NULL, __PRETTY_FUNCTION__);
        
        CDSLAPP_ << "Allocate the Data Consumer";
        data_consumer.reset(new DataConsumer(), "DataConsumer");
		if(!data_consumer.get()) throw chaos::CException(-1, "Error instantiating data consumer", __PRETTY_FUNCTION__);
		data_consumer->settings = &settings;
		
		CDSLAPP_ << "Link class";
		data_consumer->network_broker = network_broker.get();

        data_consumer.init(NULL, __PRETTY_FUNCTION__);
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
		network_broker.start(__PRETTY_FUNCTION__);
        data_consumer.start(__PRETTY_FUNCTION__);
		
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
    CDSLAPP_ << "Stop the Data Consumer";
    data_consumer.stop(__PRETTY_FUNCTION__);
	
	CDSLAPP_ << "Stopping CHAOS Data Service";
    network_broker.stop(__PRETTY_FUNCTION__);
}

/*
 Deiniti all the manager
 */
void ChaosDataService::deinit() throw(CException) {
	
	if(data_consumer.get()) {
		CDSLAPP_ << "Stop the Data Consumer";
		data_consumer.deinit(__PRETTY_FUNCTION__);
		CDSLAPP_ << "Release the endpoint associated to the Data Consumer";
		network_broker->releaseDirectIOServerEndpoint(data_consumer->server_endpoint);
	}
	
	if(network_broker.get()) {
		CDSLAPP_ << "Deinitializing CHAOS Data Service";
		network_broker.deinit(__PRETTY_FUNCTION__);
	}
    CDSLAPP_ << "Deinitializated";
}


/*
 *
 */
void ChaosDataService::signalHanlder(int signalNumber) {
    waitCloseSemaphore.unlock();
}

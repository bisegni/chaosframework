/*	
 *	ChaosMetadataService.cpp
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
#include "ChaosMetadataService.h"
#include <csignal>
#include <chaos/common/exception/CException.h>

using namespace std;
using namespace chaos;
using namespace chaos::nd;
using boost::shared_ptr;

WaitSemaphore ChaosMetadataService::waitCloseSemaphore;

#define LCND_LAPP LAPP_ << "[ChaosMetadataService] - "
#define LCND_LDBG LDBG_ << "[ChaosMetadataService] - " << __PRETTY_FUNCTION << " - "
#define LCND_LERR LERR_ << "[ChaosMetadataService] - " << __PRETTY_FUNCTION << "(" << __LINE__ << ") - " 

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
            throw CException(0, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
        }
 
        if (signal((int) SIGQUIT, ChaosMetadataService::signalHanlder) == SIG_ERR) {
            throw CException(0, "Error registering SIG_ERR signal", __PRETTY_FUNCTION__);
        }
        
        network_broker_service.reset(new NetworkBroker(), "NetworkBroker");
        network_broker_service.init(NULL, __PRETTY_FUNCTION__);
        
        network_broker_service->registerAction(&deviceApi);
		
		LAPP_ << "-----------------------------------------";
		LAPP_ << "!CHAOS Metadata service stardet";
		LAPP_ << "RPC Server address: " << network_broker_service->getRPCUrl();
		LAPP_ << "DirectIO Server address: " << network_broker_service->getDirectIOUrl();
		LAPP_ << "Sync RPC URL: " << network_broker_service->getSyncRPCUrl();
		LAPP_ << "-----------------------------------------";
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
        network_broker_service.start(__PRETTY_FUNCTION__);
        
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
    //stop network brocker
    network_broker_service.stop(__PRETTY_FUNCTION__);
    
    //endWaithCondition.notify_one();
    waitCloseSemaphore.unlock();
}

/*
 Deiniti all the manager
 */
void ChaosMetadataService::deinit()   throw(CException) {
    network_broker_service->deregisterAction(&deviceApi);

    //deinit network brocker
    network_broker_service.deinit(__PRETTY_FUNCTION__);
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

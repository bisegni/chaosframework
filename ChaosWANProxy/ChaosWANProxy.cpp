/*
 *	ChaosWANProxy.cpp
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
#include "ChaosWANProxy.h"

#include <csignal>
#include <chaos/common/exception/CException.h>

using namespace std;
using namespace chaos;
using namespace chaos::wan_proxy;
using boost::shared_ptr;

WaitSemaphore chaos::wan_proxy::ChaosWANProxy::waitCloseSemaphore;

#define LCND_LAPP LAPP_ << "[ChaosWANProxy] - "
#define LCND_LDBG LDBG_ << "[ChaosWANProxy] - " << __PRETTY_FUNCTION << " - "
#define LCND_LERR LERR_ << "[ChaosWANProxy] - " << __PRETTY_FUNCTION << "(" << __LINE__ << ") - "

//! C and C++ attribute parser
/*!
 Specialized option for startup c and cpp program main options parameter
 */
void ChaosWANProxy::init(int argc, char* argv[]) throw (CException) {
	ChaosCommon<ChaosWANProxy>::init(argc, argv);
}
//!stringbuffer parser
/*
 specialized option for string stream buffer with boost semantics
 */
void ChaosWANProxy::init(istringstream &initStringStream) throw (CException) {
	ChaosCommon<ChaosWANProxy>::init(initStringStream);
}

/*
 *
 */
void ChaosWANProxy::init(void *init_data)  throw(CException) {
	try {
		ChaosCommon<ChaosWANProxy>::init(init_data);
		if (signal((int) SIGINT, ChaosWANProxy::signalHanlder) == SIG_ERR) {
			throw CException(0, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
		}
		
		if (signal((int) SIGQUIT, ChaosWANProxy::signalHanlder) == SIG_ERR) {
			throw CException(0, "Error registering SIG_ERR signal", __PRETTY_FUNCTION__);
		}
		
		network_broker_service.reset(new NetworkBroker(), "NetworkBroker");
		network_broker_service.init(NULL, __PRETTY_FUNCTION__);
	} catch (CException& ex) {
		DECODE_CHAOS_EXCEPTION(ex)
		exit(1);
	}
	//start data manager
}

/*
 *
 */
void ChaosWANProxy::start()  throw(CException) {
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
void ChaosWANProxy::stop()   throw(CException) {
	//stop network brocker
	network_broker_service.stop(__PRETTY_FUNCTION__);
	
	//endWaithCondition.notify_one();
	waitCloseSemaphore.unlock();
}

/*
 Deiniti all the manager
 */
void ChaosWANProxy::deinit()   throw(CException) {
	
	//deinit network brocker
	network_broker_service.deinit(__PRETTY_FUNCTION__);
}

/*
 *
 */
void ChaosWANProxy::signalHanlder(int signalNumber) {
	//lock lk(monitor);
	//unlock the condition for end start method
	//endWaithCondition.notify_one();
	waitCloseSemaphore.unlock();
}

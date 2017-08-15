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
#include "ChaosWANProxy.h"
#include <chaos/common/healt_system/HealtManager.h>

#include "global_constant.h"
#include "DefaultWANInterfaceHandler.h"
#include "wan_interface/wan_interface.h"

#include <csignal>

#include <chaos/common/exception/CException.h>
#include <chaos/common/utility/StartableService.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>

using namespace std;
using namespace chaos;
using namespace chaos::common::network;
using namespace chaos::common::utility;
using namespace chaos::common::healt_system;

using namespace chaos::wan_proxy;
using namespace chaos::wan_proxy::persistence;
using ChaosSharedPtr;

WaitSemaphore chaos::wan_proxy::ChaosWANProxy::waitCloseSemaphore;

#define LCND_LAPP LAPP_ << "[ChaosWANProxy] - "
#define LCND_LDBG LDBG_ << "[ChaosWANProxy] - " << __PRETTY_FUNCTION__ << " - "
#define LCND_LERR LERR_ << "[ChaosWANProxy] - " << __PRETTY_FUNCTION__ << "(" << __LINE__ << ") - "

ChaosWANProxy::ChaosWANProxy():
wan_interface_handler(NULL) {
	
}

ChaosWANProxy::~ChaosWANProxy() {
	
}

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
	std::string tmp_interface_name;
	try {
		ChaosCommon<ChaosWANProxy>::init(init_data);
       StartableService::initImplementation(HealtManager::getInstance(), NULL, "HealtManager", __PRETTY_FUNCTION__);

		if(!getGlobalConfigurationInstance()->hasOption(setting_options::OPT_INTERFACE_TO_ACTIVATE)) {
			throw CException(-1, "The interface protocol are mandatory", __PRETTY_FUNCTION__);
		}
		
		
		if (signal((int) SIGINT, ChaosWANProxy::signalHanlder) == SIG_ERR) {
			throw CException(-2, "Error registering SIGINT signal", __PRETTY_FUNCTION__);
		}
		
		if (signal((int) SIGQUIT, ChaosWANProxy::signalHanlder) == SIG_ERR) {
			throw CException(-3, "Error registering SIG_ERR signal", __PRETTY_FUNCTION__);
		}
		
		persistence_driver.reset(new DefaultPersistenceDriver(NetworkBroker::getInstance()), "DefaultPresistenceDriver");
		persistence_driver.init(NULL, __PRETTY_FUNCTION__);
		//setting.list_cds_server.push_back(getGlobalConfigurationInstance()->getMetadataServerAddress());
		persistence_driver->addServerList(setting.list_cds_server);
		
		//Allcoate the handler
		wan_interface_handler = new DefaultWANInterfaceHandler(persistence_driver.get());
		if(!wan_interface_handler) throw CException(-5, "Error instantiating wan interface handler", __PRETTY_FUNCTION__);

		((DefaultWANInterfaceHandler*)wan_interface_handler)->registerGroup();
		
		//start all proxy interface
		for(SettingStringListIterator it = setting.list_wan_interface_to_enable.begin();
			it != setting.list_wan_interface_to_enable.end();
			it++) {
			tmp_interface_name.clear();
			tmp_interface_name = *it;
			wan_interface::AbstractWANInterface *tmp_interface_instance = ObjectFactoryRegister<wan_interface::AbstractWANInterface>::getInstance()->getNewInstanceByName(tmp_interface_name);
			if(!tmp_interface_instance) {
				LCND_LERR << "Error allocating " <<tmp_interface_name<< " wan interface";
				continue;
			}
			

			// try to initialize the implementation
			StartableService::initImplementation(tmp_interface_instance,
												 (void*)setting.parameter_wan_interfaces.c_str(),
												 tmp_interface_instance->getName(),
												 __PRETTY_FUNCTION__);
			//se the handler
			tmp_interface_instance->setHandler(wan_interface_handler);
			
			//add implemetnation to list
			wan_active_interfaces.push_back(tmp_interface_instance);
			
			LCND_LAPP << "Wan interface: " <<tmp_interface_instance->getName()<< " have been installed";

		}
		
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
	      StartableService::startImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__);
		//start all wan interface
		for(WanInterfaceListIterator it = wan_active_interfaces.begin();
			it != wan_active_interfaces.end();
			it++) {
			// try to start the implementation
			StartableService::startImplementation(*it,
												 (*it)->getName(),
												 __PRETTY_FUNCTION__);
		}
		
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
	
	//start all wan interface
	for(WanInterfaceListIterator it = wan_active_interfaces.begin();
		it != wan_active_interfaces.end();
		it++) {
		// try to start the implementation
		CHAOS_NOT_THROW(StartableService::stopImplementation(*it,
															 (*it)->getName(),
															 __PRETTY_FUNCTION__);)
	}
	  CHAOS_NOT_THROW(StartableService::stopImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););
	//endWaithCondition.notify_one();
	waitCloseSemaphore.unlock();


}

/*
 Deiniti all the manager
 */
void ChaosWANProxy::deinit()   throw(CException) {
	//deinit all wan interface
    CHAOS_NOT_THROW(StartableService::deinitImplementation(HealtManager::getInstance(), "HealtManager", __PRETTY_FUNCTION__););

	for(WanInterfaceListIterator it = wan_active_interfaces.begin();
		it != wan_active_interfaces.end();
		it++) {
		// try to deinit the implementation
		CHAOS_NOT_THROW(StartableService::deinitImplementation(*it,
															   (*it)->getName(),
															   __PRETTY_FUNCTION__);)
		//delete it
		delete(*it);
	}
	
	//clear the vector
	wan_active_interfaces.clear();
	
	if(wan_interface_handler) {
		delete(wan_interface_handler);
		wan_interface_handler = NULL;
	}
	
	persistence_driver.deinit(__PRETTY_FUNCTION__);
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

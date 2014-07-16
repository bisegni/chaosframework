/*
 *	ControlManager.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/cu_toolkit/ControlManager/ControlManager.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>

using namespace chaos;
using namespace chaos::cu;
using namespace std;

namespace chaos_data = chaos::common::data;
namespace chaos_async = chaos::common::async_central;

#define LCMAPP_ LAPP_ << "[Control Manager] - "
#define LCMDBG_ LDBG_ << "[Control Manager] - "
#define LCMERR_ LERR_ << "[Control Manager]"<<__LINE__<<" - "
#define WAITH_TIME_FOR_CU_REGISTRATION 2000000
#define WU_IDENTIFICATION(x) std::string(x->getCUInstance()) + std::string("-") + std::string(x->getCUID())


/*
 Constructor
 */
ControlManager::ControlManager() {}

/*
 Desctructor
 */
ControlManager::~ControlManager() {
}


/*
 Initialize the CU Instantiator
 */
void ControlManager::init(void *initParameter) throw(CException) {
	//control manager action initialization
    
	AbstActionDescShrPtr actionDescription;
	use_unit_server =	GlobalConfiguration::getInstance()->hasOption(CONTROL_MANAGER_UNIT_SERVER_ENABLE) &&
	GlobalConfiguration::getInstance()->getOption<bool>(CONTROL_MANAGER_UNIT_SERVER_ENABLE);
    if(use_unit_server) {
		LCMAPP_  << "Enable unit server";
		
		if(!GlobalConfiguration::getInstance()->hasOption(CONTROL_MANAGER_UNIT_SERVER_ALIAS)) {
			throw CException(-1, "No server alias param found", __PRETTY_FUNCTION__);
		}
		
		unit_server_alias = GlobalConfiguration::getInstance()->getOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS);
		
		//init CU action
		actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
																					   &ControlManager::loadControlUnit,
																					   ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN,
																					   ChaosSystemDomainAndActionLabel::ACTION_CU_LOAD,
																					   "Control Unit load system action");
		//add parameter for control unit name
		actionDescription->addParam(ChaosSystemDomainAndActionLabel::PARAM_CU_LOAD_UNLOAD_ALIAS,
									DataType::TYPE_STRING,
									"Alias of the control unit to load");
		//add parameter for driver initialization string
		actionDescription->addParam(ChaosSystemDomainAndActionLabel::PARAM_CU_LOAD_DRIVER_PARAMS,
									DataType::TYPE_STRING,
									"Driver params (pipe separated '|') to pass to the control unit instance");
		
		//deinit CU action
		
		actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
																					   &ControlManager::unloadControlUnit,
																					   ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN,
																					   ChaosSystemDomainAndActionLabel::ACTION_CU_UNLOAD,
																					   "Control Unit unload system action");
		//add parameter for control unit name
		actionDescription->addParam(ChaosSystemDomainAndActionLabel::PARAM_CU_LOAD_UNLOAD_ALIAS,
									DataType::TYPE_STRING,
									"Alias of the control unit to unload");
		
		//add unit server registration managment timer
		chaos_async::AsyncCentralManager::getInstance()->addTimer(this, 0, GlobalConfiguration::getInstance()->getOption<uint64_t>(CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC));
	}
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                   &ControlManager::updateConfiguration,
                                                                                   "commandManager",
                                                                                   "updateConfiguration",
                                                                                   "Update Command Manager Configuration");
    
	//register command manager action
    CommandManager::getInstance()->registerAction(this);
    
    LCMAPP_ << "Get the Metadataserver channel";
    mdsChannel = CommandManager::getInstance()->getMetadataserverChannel();
    if(mdsChannel) LCMAPP_ << "Metadataserver has been allocated";
    else  LCMAPP_ << "Metadataserver allocation failed";
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::start() throw(CException) {
    LCMAPP_  << "Start cu scan timer";
	thread_run = true;
	thread_registration.reset(new boost::thread(boost::bind(&ControlManager::manageControlUnit, this)));
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::stop() throw(CException) {
    LCMAPP_  << "Stop cu scan timer";
	thread_run = false;
	thread_waith_semaphore.unlock();
	if(thread_registration.get()) thread_registration->join();
}

/*
 Deinitialize the CU Instantiator
 */
void ControlManager::deinit() throw(CException) {
    bool detachFake = false;
    std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
    vector<string> allCUDeviceIDToStop;
    LCMAPP_  << "Deinit the Control Manager";
    LCMAPP_  << "system action deinitialization";
	//deregistering the action
    CommandManager::getInstance()->deregisterAction(this);
    LCMAPP_  << "system action deinitialized";
    
    
    LCMAPP_  << "Deinit all the submitted Control Unit";
    map<string, shared_ptr<WorkUnitInfo> >::iterator cuIter = map_control_unit_instance.begin();
    for ( ; cuIter != map_control_unit_instance.end(); cuIter++ ){
        shared_ptr<WorkUnitInfo> cu = (*cuIter).second;
        
        LCMAPP_  << "Deregister RPC action for cu whith instance:" << WU_IDENTIFICATION(cu->work_unit_instance);
        cu->work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
        for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
            CommandManager::getInstance()->deregisterAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
        }
        
		//load all device id for this cu
        allCUDeviceIDToStop.push_back(cu->work_unit_instance->getDeviceID());
        
        for (vector<string>::iterator iter =  allCUDeviceIDToStop.begin();
             iter != allCUDeviceIDToStop.end();
             iter++) {
            
			//stop all itnerna device
            
            CDataWrapper fakeDWForDeinit;
            fakeDWForDeinit.addStringValue(DatasetDefinitionkey::DEVICE_ID, *iter);
            try{
                LCMAPP_  << "Stopping Control Unit: " << WU_IDENTIFICATION(cu->work_unit_instance);
                cu->work_unit_instance->_stop(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
					//these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            
            try{
                LCMAPP_  << "Deiniting Control Unit: " << WU_IDENTIFICATION(cu->work_unit_instance);
                cu->work_unit_instance->_deinit(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
					//these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            try{
				LCMAPP_  << "Undefine Action And Dataset for  Control Unit: " << WU_IDENTIFICATION(cu->work_unit_instance);
                cu->work_unit_instance->_undefineActionAndDataset();
            }  catch (CException& ex) {
                if(ex.errorCode != 1){
					//these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
        }
        LCMAPP_  << "Dispose event channel for Control Unit Sanbox:" << WU_IDENTIFICATION(cu->work_unit_instance);
        CommandManager::getInstance()->deleteEventChannel(cu->work_unit_instance->deviceEventChannel);
        cu->work_unit_instance->deviceEventChannel = NULL;
        cuDeclareActionsInstance.clear();
        LCMAPP_  << "Unload" << cu->work_unit_instance->getCUInstance();
    }
    map_control_unit_instance.clear();
}



/*
 Submit a new Control unit for operation
 */
void ControlManager::submitControlUnit(AbstractControlUnit *data) throw(CException) {
    boost::unique_lock<boost::shared_mutex> lock(mutex_registration);
    submittedCUQueue.push(data);
	
	//unlock thread
	thread_waith_semaphore.unlock();
}

void ControlManager::manageControlUnit() {
	//initialize the Control Unit
    int registrationError = ErrorCode::EC_NO_ERROR;
    AbstractControlUnit *curCU = 0L;
    std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
    CDataWrapper cuActionAndDataset;
	
	boost::unique_lock<boost::shared_mutex> lock(mutex_registration, boost::defer_lock);
	while(thread_run) {
		while(!submittedCUQueue.empty()){
			//lock queue
			lock.lock();
			
			//get the oldest data ad copy the shared_ptr
			curCU = submittedCUQueue.front();
			
			//remove the oldest data
			submittedCUQueue.pop();
			
			//unlock queue
			lock.unlock();
			try {
				LCMAPP_  << "Got new Control Unit";
				shared_ptr<WorkUnitInfo> wui(new WorkUnitInfo(curCU));
				
				//associate the event channel to the control unit
				LCMAPP_  << "Adding event channel to the control unit:" << WU_IDENTIFICATION(wui->work_unit_instance);
				wui->work_unit_instance->deviceEventChannel = CommandManager::getInstance()->getDeviceEventChannel();
				
				LCMAPP_  << "Setup Control Unit Sanbox for cu with instance:" << WU_IDENTIFICATION(wui->work_unit_instance);
				wui->work_unit_instance->_defineActionAndDataset(cuActionAndDataset);
				
				LCMAPP_  << "Register RPC action for cu whith instance:" << WU_IDENTIFICATION(wui->work_unit_instance);
				wui->work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
				for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
					CommandManager::getInstance()->registerAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
				}
				
				//sendConfPackToMDS(cuPtr->defaultInternalConf.get());
				LCMAPP_  << "Talk with MDS for cu with instance:" << WU_IDENTIFICATION(wui->work_unit_instance);
				registrationError = sendConfPackToMDS(cuActionAndDataset);
				if(registrationError == ErrorCode::EC_NO_ERROR){
					LCMAPP_  << "Configuration pack has been sent to MDS for cu with instance:" << WU_IDENTIFICATION(wui->work_unit_instance);
					LCMAPP_  << "Control Unit Sanbox:" << WU_IDENTIFICATION(wui->work_unit_instance) << " ready to work";
				} else {
					LCMAPP_  << "ERROR sending configuration pack has been sent to MDS for cu with instance:" << WU_IDENTIFICATION(wui->work_unit_instance);
				}
				
				//the sandbox name now is the real CUName_CUInstance before the initSandbox method call the CUInstance is
				//randomlly defined but if a CU want to ovveride it it can dureing initSandbox call
				if(map_control_unit_instance.count(wui->work_unit_instance->getCUInstance())) {
					LCMERR_  << "Duplicated control unit instance " << WU_IDENTIFICATION(wui->work_unit_instance);
					return;
				}
				
				//add sandbox to all map of running cu
				
				map_control_unit_instance.insert(make_pair(wui->work_unit_instance->getCUInstance(), wui));
				
				//check if we need to autostart and init the CU
				if(cuActionAndDataset.hasKey(CUDefinitionKey::CS_CM_CU_AUTOSTART) &&
				   cuActionAndDataset.getInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART)){
					//cuPtr->initSandbox(cuPtr->defaultInternalConf.get());
					auto_ptr<SerializationBuffer> serBuffForGlobalConf(GlobalConfiguration::getInstance()->getConfiguration()->getBSONData());
					auto_ptr<CDataWrapper> masterConfiguration(new CDataWrapper(serBuffForGlobalConf->getBufferPtr()));
					masterConfiguration->appendAllElement(cuActionAndDataset);
					
#if DEBUG
					LDBG_ << "Registration Error:" << registrationError;
					LDBG_ << masterConfiguration->getJSONString();
#endif
				}
				
				
				//clear
				cuDeclareActionsInstance.clear();
				cuActionAndDataset.reset();
			} catch (CException& ex) {
				DECODE_CHAOS_EXCEPTION(ex)
			}
			//no
		}
		
		//all cu as been registered, wiath for other task
		thread_waith_semaphore.wait();
	}
}

/*
 
 */
int ControlManager::sendConfPackToMDS(CDataWrapper& dataToSend) {
	// dataToSend can't be sent because it is porperty of the CU
	//so we need to copy it
    
    auto_ptr<SerializationBuffer> serBuf(dataToSend.getBSONData());
    CDataWrapper *mdsPack = new CDataWrapper(serBuf->getBufferPtr());
	//add action for metadata server
	//add local ip and port
    
    mdsPack->addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS, GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort().c_str());
    
	//register CU from mds
    return mdsChannel->sendUnitDescription(mdsPack, true, WAITH_TIME_FOR_CU_REGISTRATION);
}


//! message for load operation
CDataWrapper* ControlManager::loadControlUnit(CDataWrapper *messageData, bool& detach) throw (CException) {
    return NULL;
}

//! message for unload operation
CDataWrapper* ControlManager::unloadControlUnit(CDataWrapper *messageData, bool& detach) throw (CException) {
    return NULL;
}

/*
 Configure the sandbox and all subtree of the CU
 */
CDataWrapper* ControlManager::updateConfiguration(CDataWrapper *param_info, bool& detach) {
    return NULL;
}

//---------------unit server state machine managment handler
void ControlManager::timeout() {
	boost::unique_lock<boost::shared_mutex> lock_sm(unit_server_sm_mutex);
	switch (unit_server_sm.current_state()[0]) {
			//Unpublished
		case 0:
			LCMDBG_ << "[Unpublished] Send first registration pack to mds";
			if(use_unit_server) {
				if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypePublishing()) == boost::msm::back::HANDLED_TRUE){
					//gone to publishing
					sendUnitServerRegistration();
				} else {
					LCMERR_ << "[Unpublished] i can't be here";
				}
			} else {
				LCMDBG_ << "[Publishing] Unit server registration not sucessfull, turn off the timer";
				chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
			}
			break;
			//Publishing
		case 1:
			LCMDBG_ << "[Publishing] Send another registration pack to mds";
			sendUnitServerRegistration();
			break;
			//Published
		case 2:
			LCMDBG_ << "[Published] Unit server registration completed, turn off the timer";
			chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
			break;
			//Published failed
		case 3:
			LCMDBG_ << "[Published failed] Perform Unpublishing state";
			chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
			use_unit_server = false;
			if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypeUnpublished()) == boost::msm::back::HANDLED_TRUE){
				LCMDBG_ << "[Published failed] got to [Unpublished with use_unit_server = "<<use_unit_server<<"]";
			} else {
				LCMERR_ << "[Published failed] i can't be here";
			}
			break;
	}
}

//!prepare and send registration pack to the metadata server
void ControlManager::sendUnitServerRegistration() {
	chaos_data::CDataWrapper unitServerRegistrationPack;
	//set server alias
	unitServerRegistrationPack.addStringValue(ChaosMetadataRPCConstants::MDS_REGISTER_UNIT_SERVER_ALIAS, unit_server_alias);
	mdsChannel->sendUnitServerRegistration(&unitServerRegistrationPack);
}

// Server registration ack message
CDataWrapper* ControlManager::unitServerRegistrationACK(CDataWrapper *messageData, bool &detach) throw (CException) {
	//lock the sm access
	boost::unique_lock<boost::shared_mutex> lock_sm(unit_server_sm_mutex);
	
	if(!messageData->hasKey(ChaosMetadataRPCConstants::MDS_REGISTER_UNIT_SERVER_ALIAS))
		throw CException(-1, "No alias forwarder", __PRETTY_FUNCTION__);
	
	string server_alias = messageData->getStringValue(ChaosMetadataRPCConstants::MDS_REGISTER_UNIT_SERVER_ALIAS);
	if(server_alias.compare(unit_server_alias) != 0) {
		throw CException(-2, "Server alias not found", __PRETTY_FUNCTION__);
	}
	if(messageData->hasKey(ChaosMetadataRPCConstants::MDS_REGISTER_UNIT_SERVER_RESULT)) {
		switch(messageData->getInt32Value(ChaosMetadataRPCConstants::MDS_REGISTER_UNIT_SERVER_RESULT)){
			case ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_OK:
				if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypeUnpublished()) == boost::msm::back::HANDLED_TRUE){
					//we are published and it is ok!
				} else {
					throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for published event", __PRETTY_FUNCTION__);
				}
				break;
				
			case ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_FAILURE_DUPLICATE_ALIAS:
				LCMERR_ << "The " << unit_server_alias << " is already used";
				//turn of unit server
				if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypeUnpublishing()) == boost::msm::back::HANDLED_TRUE){
					//we are published and it is ok!
				} else {
					throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for unpublishing event", __PRETTY_FUNCTION__);
				}
				break;
				
			case ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_FAILURE_INVALID_ALIAS:
				LCMERR_ << "The " << unit_server_alias << " is invalid";
				//turn of unit server
				LCMDBG_ << "Turning of unit server";
				use_unit_server = false;
				if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypeUnpublishing()) == boost::msm::back::HANDLED_TRUE){
					//we are published and it is ok!
				} else {
					throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for unpublished event", __PRETTY_FUNCTION__);
				}
				break;
		}
		
	} else {
		throw CException(-3, "No result received", __PRETTY_FUNCTION__);
	}
	return NULL;
}
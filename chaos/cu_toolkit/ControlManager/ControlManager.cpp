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

#define LCMAPP_ LAPP_ << "[Control Manager] - "
#define LCMDBG_ LDBG_ << "[Control Manager] - "
#define LCMERR_ LERR_ << "[Control Manager] - "
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
    LCMAPP_  << "Inititializing";
    
    
	//control manager action initialization
    LCMAPP_  << "system action initialization";
    
	//init CU action
    AbstActionDescShrPtr
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

/*
 Load the requested control unit
 */
CDataWrapper* ControlManager::loadControlUnit(CDataWrapper *param_info, bool& detach) throw (CException) {
    return NULL;
}

/*
 Unload the requested control unit
 */
CDataWrapper* ControlManager::unloadControlUnit(CDataWrapper *param_info, bool& detach) throw (CException) {
    return NULL;
}

/*
 Configure the sandbox and all subtree of the CU
 */
CDataWrapper* ControlManager::updateConfiguration(CDataWrapper *param_info, bool& detach) {
    return NULL;
}

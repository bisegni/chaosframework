/*
 *	WorkUnitManagement.cpp
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

#include <chaos/cu_toolkit/ControlManager/WorkUnitManagement.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>

#define WUMHADER "[WorkUnitManagement " << std::string(work_unit_instance->getCUInstance()) + std::string("-") + std::string(work_unit_instance->getCUID()) << "] - "
#define WUMAPP_ LAPP_ << WUMHADER
#define WUMDBG_ LDBG_ << WUMHADER
#define WUMERR_ LERR_ << WUMHADER<<"("<<__LINE__<<") "

using namespace chaos::common::data;
using namespace chaos::cu::control_manager;

WorkUnitManagement::WorkUnitManagement(AbstractControlUnit *_work_unit_instance):
mds_channel(NULL),
work_unit_instance(_work_unit_instance) {
}

WorkUnitManagement::~WorkUnitManagement(){
}

//! return the state of the unit state machine
UnitState WorkUnitManagement::getCurrentState() {
	return (UnitState) wu_instance_sm.current_state()[0];
}
	
void WorkUnitManagement::scheduleSM() throw (CException) {
	WUMDBG_ << "Shedule state machine step"
	switch ((UnitState) wu_instance_sm.current_state()[0]) {
		case UnitState::UnitStateUnpublished: {
			CDataWrapper cuActionAndDataset;
			std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
			WUMDBG_ << "Control unit is unpublished, need to be setup"
			//associate the event channel to the control unit
			WUMDBG_  << "Adding event channel";
			work_unit_instance->deviceEventChannel = CommandManager::getInstance()->getDeviceEventChannel();
			
			LCMAPP_  << "Setup Control Unit Sanbox for cu with instance"
			work_unit_instance->_defineActionAndDataset(cuActionAndDataset);
			
			LCMAPP_  << "Register RPC action for cu whith instance";
			work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
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
			break;
		}
			
		case UnitState::UnitStatePublishing:
			
			break;
		case UnitState::UnitStatePublished:
			
			break;
		case UnitState::UnitStatePublishingFailure:
			
			break;
		case UnitState::UnitStateUnpublishing:
			
			break;
	}
}

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

void WorkUnitManagement::manageACKPack(CDataWrapper ackPack) {
	
}
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
using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;

WorkUnitManagement::WorkUnitManagement(AbstractControlUnit *_work_unit_instance):
mds_channel(NULL),
work_unit_instance(_work_unit_instance),
active(false) {
}

WorkUnitManagement::~WorkUnitManagement(){
}

//! return the state of the unit state machine
UnitState WorkUnitManagement::getCurrentState() {
	return (UnitState) wu_instance_sm.current_state()[0];
}

//! turn on the control unit
void WorkUnitManagement::turnOn() throw (CException) {
	active = true;
	if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypePublish()) == boost::msm::back::HANDLED_TRUE){
		//we are switched state
	} else {
		throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for UnitEventTypePublish event", __PRETTY_FUNCTION__);
	}
}

//! turn off the control unit
void WorkUnitManagement::turnOFF() throw (CException) {
	active = false;
	if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublish()) == boost::msm::back::HANDLED_TRUE){
		//we are switched state
	} else {
		throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for UnitEventTypeUnpublish event", __PRETTY_FUNCTION__);
	}

}

void WorkUnitManagement::scheduleSM() throw (CException) {
	WUMDBG_ << "Schedule state machine step";
	switch ((UnitState) wu_instance_sm.current_state()[0]) {
		case UnitStateUnpublished:
//			WUMAPP_ << "[UnitStateUnpublished] Control unit is unpublished"

			break;
			
		case UnitStateStartPublishing: {
			std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
			WUMAPP_ << "[UnitStateStartPublishing] Control unit is unpublished, need to be setup";
			//associate the event channel to the control unit
			WUMAPP_ << "[UnitStateStartPublishing] Adding event channel";
			work_unit_instance->deviceEventChannel = CommandManager::getInstance()->getDeviceEventChannel();
			
			WUMAPP_ << "[UnitStateStartPublishing] Setup Control Unit Sanbox for cu with instance";
			work_unit_instance->_defineActionAndDataset(mds_registration_message);
			
			WUMAPP_ << "[UnitStateStartPublishing] Register RPC action for cu whith instance";
			work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
			for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
				CommandManager::getInstance()->registerAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
			}
			
			//sendConfPackToMDS(cuPtr->defaultInternalConf.get());
			if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypePublishing()) == boost::msm::back::HANDLED_TRUE){
				//we are switched state
			} else {
				throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for unpublishing event", __PRETTY_FUNCTION__);
			}
			break;
		}
			
		case UnitStatePublishing: {
			WUMAPP_  << "[UnitStatePublishing] send registration to mds";
			if(sendConfPackToMDS(mds_registration_message)) {
				WUMERR_ << "Error forwarding registration message to mds";
			}
			break;
		}
			
		case UnitStatePublished:
			WUMAPP_  << "[Published] work unit has been successfully published";
			break;
			
		case UnitStateStartUnpublishing:
			WUMAPP_  << "[Published] work unit has been successfully published";
			break;
			
		case UnitStatePublishingFailure:
			WUMAPP_  << "[UnitStatePublishingFailure] there was been error during control unit registration we end here";
			break;
			
		case UnitStateUnpublishing:
			WUMAPP_  << "[UnitStateUnpublishing] work unit is going to be unpublished";
			break;
	}
}

bool WorkUnitManagement::smNeedToSchedule() {
	UnitState s = getCurrentState();
	return	s != UnitStatePublished &&
			s != UnitStateUnpublished &&
			s != UnitStatePublishingFailure;
			
}

int WorkUnitManagement::sendConfPackToMDS(CDataWrapper& dataToSend) {
	// dataToSend can't be sent because it is porperty of the CU
	//so we need to copy it
    
    auto_ptr<SerializationBuffer> serBuf(dataToSend.getBSONData());
    CDataWrapper *mdsPack = new CDataWrapper(serBuf->getBufferPtr());
	//add action for metadata server
	//add local ip and port
    
    mdsPack->addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS, GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort().c_str());
    
	//register CU from mds
    return mds_channel->sendUnitDescription(mdsPack);
}

void WorkUnitManagement::manageACKPack(CDataWrapper& ackPack) {
	
}
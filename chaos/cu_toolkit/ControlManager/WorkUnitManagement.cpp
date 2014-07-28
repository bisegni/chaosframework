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

#define WUMHADER "[WorkUnitManagement " << std::string(work_unit_instance->getCUInstance()) + std::string("-") + std::string(work_unit_instance->getCUID()) << "-"<<getCurrentStateString()<<"] - "
#define WUMAPP_ LAPP_ << WUMHADER
#define WUMDBG_ LDBG_ << WUMHADER
#define WUMERR_ LERR_ << WUMHADER<<"("<<__LINE__<<") "

using namespace chaos::common::data;
using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;

/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
WorkUnitManagement::WorkUnitManagement(AbstractControlUnit *_work_unit_instance):
mds_channel(NULL),
work_unit_instance(_work_unit_instance),
active(false) {
}

/*---------------------------------------------------------------------------------

---------------------------------------------------------------------------------*/
WorkUnitManagement::~WorkUnitManagement(){
}

/*---------------------------------------------------------------------------------
 return the state of the unit state machine
 ---------------------------------------------------------------------------------*/
UnitState WorkUnitManagement::getCurrentState() {
	return (UnitState) wu_instance_sm.current_state()[0];
}

/*---------------------------------------------------------------------------------
 return the state of the unit state machine
 ---------------------------------------------------------------------------------*/
string WorkUnitManagement::getCurrentStateString() {
	std::string result;
	switch (getCurrentState()) {
		case UnitStatePublished:
			result = "Published";
			break;
		case UnitStatePublishingFailure:
			result = "PublishingFailure";
			break;
		case UnitStatePublishing:
			result = "Publishing";
			break;
		case UnitStateStartPublishing:
			result = "StartPublishing";
			break;
		case UnitStateStartUnpublishing:
			result = "StartUnpublishing";
			break;
		case UnitStateUnpublished:
			result = "Unpublished";
			break;
		case UnitStateUnpublishing:
			result = "Unpublishing";
			break;
	}
	return result;
}

/*---------------------------------------------------------------------------------
 turn on the control unit
 ---------------------------------------------------------------------------------*/
void WorkUnitManagement::turnOn() throw (CException) {
	if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypePublish()) == boost::msm::back::HANDLED_TRUE){
		//we are switched state
	} else {
		throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for UnitEventTypePublish event", __PRETTY_FUNCTION__);
	}
}

/*---------------------------------------------------------------------------------
 turn off the control unit
 ---------------------------------------------------------------------------------*/
void WorkUnitManagement::turnOFF() throw (CException) {
	if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublish()) == boost::msm::back::HANDLED_TRUE){
		//we are switched state
	} else {
		throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for UnitEventTypeUnpublish event", __PRETTY_FUNCTION__);
	}

}

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
void WorkUnitManagement::scheduleSM() throw (CException) {
	WUMDBG_ << "Start state machine step";
	switch ((UnitState) wu_instance_sm.current_state()[0]) {
		case UnitStateUnpublished:
			WUMAPP_ << "[work unit in unpublished";
			active = false;

			break;
			
		case UnitStateStartPublishing: {
			active = true;
			WUMAPP_ << "Control unit is unpublished, need to be setup";
			//associate the event channel to the control unit
			WUMAPP_ << "Adding event channel";
			work_unit_instance->deviceEventChannel = CommandManager::getInstance()->getDeviceEventChannel();
			
			WUMAPP_ << "Setup Control Unit Sanbox for cu with instance";
			work_unit_instance->_defineActionAndDataset(mds_registration_message);
			
			//sendConfPackToMDS(cuPtr->defaultInternalConf.get());
			if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypePublishing()) == boost::msm::back::HANDLED_TRUE){
				//we are switched state
			} else {
				throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for publishing event", __PRETTY_FUNCTION__);
			}
			break;
		}
			
		case UnitStatePublishing: {
			WUMAPP_  << "send registration to mds";
			if(sendConfPackToMDS(mds_registration_message)) {
				WUMERR_ << "Error forwarding registration message to mds";
			}
			break;
		}
			
		case UnitStatePublished: {
			active = false;
			WUMAPP_ << "work unit has been successfully published";
			WUMAPP_ << "Register RPC action for cu whith instance";
			std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
			work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
			for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
				CommandManager::getInstance()->registerAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
			}
			break;
		}
		case UnitStateStartUnpublishing: {
			active = true;
			WUMAPP_ << "work unit is starting the unpublishing porcess";
			WUMAPP_ << "Register RPC action for cu whith instance";
			std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
			work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
			for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
				CommandManager::getInstance()->deregisterAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
			}
			if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublishing()) == boost::msm::back::HANDLED_TRUE){
				//we are switched state
			} else {
				throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for unpublish event", __PRETTY_FUNCTION__);
			}
			break;
		}
			
		case UnitStatePublishingFailure:
			active = false;
			WUMAPP_  << "there was been error during control unit registration we end here";
			break;
			
		case UnitStateUnpublishing:
			WUMAPP_  << "work unit is going to be unpublished";
			if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublished()) == boost::msm::back::HANDLED_TRUE){
				//we are switched state
			} else {
				throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for unpublishing event", __PRETTY_FUNCTION__);
			}
			break;
	}
	WUMDBG_ << "End state machine step";
}
bool WorkUnitManagement::smNeedToSchedule() {
	UnitState s = getCurrentState();
	return	active || ( s != UnitStatePublished &&
						s != UnitStateUnpublished &&
						s != UnitStatePublishingFailure);
			
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

bool WorkUnitManagement::manageACKPack(CDataWrapper& ack_pack) {
	bool result = false;
	WUMAPP_ << "Work unit registration ack message received";

	if(!ack_pack.hasKey(ChaosSystemDomainAndActionLabel::PARAM_WORK_UNIT_REG_ACK_DEVICE_ID))
		throw CException(-1, "No device id found", __PRETTY_FUNCTION__);
	
	string device_id = ack_pack.getStringValue(ChaosSystemDomainAndActionLabel::PARAM_WORK_UNIT_REG_ACK_DEVICE_ID);
	if(device_id.compare(work_unit_instance->getCUID()) != 0) {
		throw CException(-2, "received id not equal to work unit instance id", __PRETTY_FUNCTION__);
	}
	if(ack_pack.hasKey(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT)) {
		//registration has been ended
		switch(ack_pack.getInt32Value(ChaosSystemDomainAndActionLabel::MDS_REGISTER_UNIT_SERVER_RESULT)){
			case ErrorCode::EC_MDS_UNIT_SERV_REGISTRATION_OK:
				WUMAPP_ << "work unit has been registered";
				if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypePublished()) == boost::msm::back::HANDLED_TRUE){
					//we are published and it is ok!
				} else {
					throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for UnitEventTypePublished event", __PRETTY_FUNCTION__);
				}
				result = true;
				break;
			
			case ErrorCode::EC_MDS_WOR_UNIT_ID_NOT_SELF_MANAGEABLE:
				WUMAPP_ << "id is not slef manageable";
			default:
				WUMERR_ << "work unit failed to register";
				//turn of unit server
				if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypeFailure()) == boost::msm::back::HANDLED_TRUE){
					//we have problem
				} else {
					throw CException(ErrorCode::EC_MDS_UNIT_SERV_BAD_US_SM_STATE, "Bad state of the sm for UnitEventTypeFailure event", __PRETTY_FUNCTION__);
				}
				break;
		}
	} else {
		WUMERR_ << "No result received";
		throw CException(-3, "No result received", __PRETTY_FUNCTION__);
	}
	return result;
}
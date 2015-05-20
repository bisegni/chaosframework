/*
 *	WorkUnitManagement.cpp
 *	!CHAOS
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

#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <chaos/cu_toolkit/ControlManager/WorkUnitManagement.h>

#include <chaos/common/healt_system/HealtManager.h>

#define WUMHADER "[" << std::string(work_unit_instance->getCUInstance()) + std::string("-") + std::string(work_unit_instance->getCUID()) << "-"<<getCurrentStateString()<<"] - "
#define WUMAPP_ INFO_LOG(WorkUnitManagement) << WUMHADER
#define WUMDBG_ DBG_LOG(WorkUnitManagement) << WUMHADER
#define WUMERR_ ERR_LOG(WorkUnitManagement) << WUMHADER

#define SWITCH_SM_TO(e)\
if(wu_instance_sm.process_event(e) != boost::msm::back::HANDLED_TRUE){\
throw CException(ErrorCode::EC_MDS_NODE_BAD_SM_STATE, "Bad state of the sm for failure event", __PRETTY_FUNCTION__);\
}

using namespace chaos::common::data;
using namespace chaos::common::healt_system;

using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;

/*---------------------------------------------------------------------------------
 
 ---------------------------------------------------------------------------------*/
WorkUnitManagement::WorkUnitManagement(AbstractControlUnit *_work_unit_instance):
mds_channel(NULL),
work_unit_instance(_work_unit_instance),
active(true),
publishing_counter_delay(0){
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
    WUMDBG_ << "Turn ON";
    if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypePublish()) == boost::msm::back::HANDLED_TRUE){
        //we are switched state
    } else {
        throw CException(ErrorCode::EC_MDS_NODE_BAD_SM_STATE, "Bad state of the sm for UnitEventTypePublish event", __PRETTY_FUNCTION__);
    }
}

/*---------------------------------------------------------------------------------
 turn off the control unit
 ---------------------------------------------------------------------------------*/
void WorkUnitManagement::turnOFF() throw (CException) {
    WUMDBG_ << "Turn OFF";
    if(wu_instance_sm.process_event(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublish()) == boost::msm::back::HANDLED_TRUE){
        //we are switched state
    } else {
        throw CException(ErrorCode::EC_MDS_NODE_BAD_SM_STATE, "Bad state of the sm for UnitEventTypeUnpublish event", __PRETTY_FUNCTION__);
    }
    
}

/*---------------------------------------------------------------------------------

 ---------------------------------------------------------------------------------*/
void WorkUnitManagement::scheduleSM() throw (CException) {
    WUMDBG_ << "Start state machine step";
    switch ((UnitState) wu_instance_sm.current_state()[0]) {
        case UnitStateUnpublished:
            WUMAPP_ << "Work unit in unpublished";
            HealtManager::getInstance()->addNodeMetricValue(work_unit_instance->getCUID(),
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_UNLOAD);
            active = false;
            break;
            
        case UnitStateStartPublishing: {
            active = true;
            //reset the delay for the forwarding of the registration datapack
            publishing_counter_delay = 0;
            WUMAPP_ << "Control unit is unpublished, need to be setup";
            //associate the event channel to the control unit

            WUMAPP_ << "Setup Control Unit Sanbox for cu with instance";
            try{
                work_unit_instance->_defineActionAndDataset(mds_registration_message);
            }catch(chaos::CException& e) {
                DECODE_CHAOS_EXCEPTION(e)
                SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypeFailure())
            }catch(...) {
                WUMERR_ << "Unexpected error during control unit definition";
                SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypeFailure())
            }
            SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypePublishing())
            break;
        }
            
        case UnitStatePublishing: {
            // don't pollute, ask every 20s
            if((publishing_counter_delay%10) == 0){
                WUMAPP_  << "send registration to mds with delay ountre to:" << publishing_counter_delay;
                if(sendConfPackToMDS(mds_registration_message)) {
                    WUMERR_ << "Error forwarding registration message to mds";
                }
            }
            publishing_counter_delay++;
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

                //set healt to load
            HealtManager::getInstance()->addNodeMetricValue(work_unit_instance->getCUID(),
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD);
            break;
        }
        case UnitStateStartUnpublishing: {
            active = true;
            WUMAPP_ << "work unit is starting the unpublishing process";
            WUMAPP_ << "Register RPC action for cu whith instance";
            std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
            work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
            for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
                CommandManager::getInstance()->deregisterAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
            }
            SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublishing())
            break;
        }
            
        case UnitStatePublishingFailure: {
            WUMAPP_  << "there was been error during control unit registration we end here";
            active = false;
            break;
        }
        case UnitStateUnpublishing: {
            CDataWrapper fakeDWForDeinit;
            bool detachFake;
            fakeDWForDeinit.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, work_unit_instance->getCUID());
            try{
                WUMAPP_  << "Stopping Wor Unit";
                work_unit_instance->_stop(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
                    //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }

            try{
                WUMAPP_  << "Deiniting Work Unit";
                work_unit_instance->_deinit(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
                    //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            try{
                WUMAPP_  << "Undefine Action And Dataset for  Work Unit";
                work_unit_instance->_undefineActionAndDataset();
            }  catch (CException& ex) {
                if(ex.errorCode != 1){
                    //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            
            WUMAPP_  << "work unit is going to be unpublished";
            SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypeUnpublished())
                //set healt to unload
            HealtManager::getInstance()->addNodeMetricValue(work_unit_instance->getCUID(),
                                                            NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                            NodeHealtDefinitionValue::NODE_HEALT_STATUS_UNLOAD);
            break;
        }
    }
        //publish if something has changed
    HealtManager::getInstance()->publishNodeHealt(work_unit_instance->getCUID());
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
    CDataWrapper mdsPack(serBuf->getBufferPtr());
    //add action for metadata server
    //add local ip and port

        //add default node value
        //mdsPack.addStringValue(NodeDefinitionKey::NODE_RPC_ADDR, GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort().c_str());
        //mdsPack.addInt64Value(NodeDefinitionKey::NODE_TIMESTAMP, chaos::common::utility::TimingUtil::getTimeStamp());

    //register CU from mds
    return mds_channel->sendNodeRegistration(mdsPack);
}

bool WorkUnitManagement::manageACKPack(CDataWrapper& ack_pack) {
    bool result = false;
    WUMAPP_ << "Work unit registration ack message received";
    
    if(!ack_pack.hasKey(NodeDefinitionKey::NODE_UNIQUE_ID))
        throw CException(-1, "No device id found", __PRETTY_FUNCTION__);
    
    string device_id = ack_pack.getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if(device_id.compare(work_unit_instance->getCUID()) != 0) {
        throw CException(-2, "received id :"+std::string(work_unit_instance->getCUID())+ std::string(" not equal to work unit instance id:")+device_id, __PRETTY_FUNCTION__);
    }
    if(ack_pack.hasKey(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT)) {
        //registration has been ended
        
        int ack_val=ack_pack.getInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT);
        switch(ack_val){
            case ErrorCode::EC_MDS_NODE_REGISTRATION_OK:
                SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypePublished())
                WUMAPP_ << "work unit has been registered:"<<device_id;
                result = true;
                break;
                
            case ErrorCode::EC_MDS_NODE_ID_NOT_SELF_MANAGEABLE:
                WUMAPP_ << "id is not self manageable";
            default:
                WUMERR_ << "work unit "<< device_id<<" failed to register, error ack:"<<ack_val;
                SWITCH_SM_TO(work_unit_state_machine::UnitEventType::UnitEventTypeFailure())
                
                break;
        }
    } else {
        WUMERR_ << "No result received";
        throw CException(-3, "No result received", __PRETTY_FUNCTION__);
    }
    return result;
}

/*
 *	UIToolkitCMDLineExample.cpp
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
#include <iostream>
#include <string>
#include <vector>
#include <chaos/common/global.h>
#include <chaos/common/utility/TimingUtil.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/network/CNodeNetworkAddress.h>
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>
#include <stdio.h>
#include <chaos/common/bson/bson.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace bson;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::date_time;
namespace chaos_batch = chaos::common::batch_command;

#define OPT_STATE           "op"
#define OPT_TIMEOUT         "timeout"
#define OPT_CU_ID			"deviceid"
#define OPT_SCHEDULE_TIME   "stime"
#define OPT_PRINT_STATE     "print-state"
#define OPT_PRINT_TYPE		"print-type"
#define OPT_GET_DS_VALUE	"get_ds_value"
//--------------slow contorol option----------------------------------------------------
#define OPT_SL_ALIAS									"sc-alias"
#define OPT_SL_EXEC_CHANNEL								"sc-exec-channel"
#define OPT_SL_PRIORITY									"sc-priority"
#define OPT_SL_SUBMISSION_RULE							"sc-sub-rule"
#define OPT_SL_COMMAND_DATA								"sc-cmd-data"
#define OPT_SL_COMMAND_ID								"sc-cmd-id"
#define OPT_SL_COMMAND_SCHEDULE_DELAY					"sc-cmd-sched-wait"
#define OPT_SL_COMMAND_SUBMISSION_RETRY_DELAY			"sc-cmd-submission-retry-delay"
#define OPT_SL_COMMAND_SET_FEATURES_LOCK				"sc-cmd-features-lock"
#define OPT_SL_COMMAND_SET_FEATURES_SCHEDULER_WAIT		"sc-cmd-features-sched-wait"
//--------------rt control unit option--------------------------------------------------
#define OPT_RT_ATTRIBUTE_VALUE							"rt-attr-val"

inline ptime utcToLocalPTime(ptime utcPTime){
	c_local_adjustor<ptime> utcToLocalAdjustor;
	ptime t11 = utcToLocalAdjustor.utc_to_local(utcPTime);
	return t11;
}

void print_state(CUStateKey::ControlUnitState state) {
    switch (state) {
        case CUStateKey::INIT:
            std::cout << "Initialized";
            break;
        case CUStateKey::START:
            std::cout << "Started";
            break;
        case CUStateKey::STOP:
            std::cout << "Stopped";
            break;
        case CUStateKey::DEINIT:
            std::cout << "Deinitilized";
            break;
    }
}

int checkSubmissionRule(std::string scSubmissionRule) {
    if( scSubmissionRule.compare("normal") == 0) {
        return chaos_batch::SubmissionRuleType::SUBMIT_NORMAL;
    } else if( scSubmissionRule.compare("stack")  == 0) {
        return chaos_batch::SubmissionRuleType::SUBMIT_AND_Stack;
    } else if( scSubmissionRule.compare("kill")  == 0) {
        return chaos_batch::SubmissionRuleType::SUBMIT_AND_Kill;
    } else return -1;
}

int main (int argc, char* argv[] )
{
    try {
        int err = 0;
        int op =-1;
        bool printState = false;
		bool printType = false;
        long scheduleTime;
		uint32_t timeout;
		
        string deviceID;
        string scAlias;
        string scSubmissionRule;
        string scUserData;
		uint64_t scCmdID;
        uint32_t scSubmissionPriority;
        uint32_t scSubmissionSchedulerDelay;
        uint32_t scSubmissionSubmissionRetryDelay;
        uint32_t scExecutionChannel;
		string rtAttributeValue;
		vector<string> key_to_show;
		
		
		bool scFeaturesLock;
		uint32_t scFeaturesSchedWait;
		
        CDeviceNetworkAddress deviceNetworkAddress;
        CUStateKey::ControlUnitState deviceState;
		
        //! [UIToolkit Attribute Init]
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_CU_ID, "The identification string of the device");
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_TIMEOUT, "Timeout rpc in milliseconds", 2000, &timeout);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<int>(OPT_STATE, "The state to set on the device{1=init, 2=start, 3=stop, 4=deinit, 5=set schedule time, 6=submite slow command(slcu), 7=kill current command(slcu), 8=get command state by id, 9=set input channel(rtcu)}, 10=flush history state(slcu), 11=get dataset value for keys", 0);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<long>(OPT_SCHEDULE_TIME, "the time in microseconds for devide schedule time");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<bool>(OPT_PRINT_STATE, "Print the state of the device", false, &printState);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<bool>(OPT_PRINT_TYPE, "Print the type of the control unit of the device", false, &printType);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption< vector<string> >(OPT_GET_DS_VALUE, "Print last value of the dataset keys[to use with opcode 11]", &key_to_show, true);
		
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_SL_ALIAS, "The alias associted to the command for the slow control cu", "", &scAlias);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_SL_EXEC_CHANNEL, "TThe alias used to execute the command [it's 1 based, 0 let choice the channel to the engine]", 0, &scExecutionChannel);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_SL_SUBMISSION_RULE, "The rule used for submit the command for the slow control cu [normal, stack, kill]","stack", &scSubmissionRule);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_SL_PRIORITY, "The priority used for submit the command for the slow control cu", 50,&scSubmissionPriority);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_SL_COMMAND_DATA, "The bson pack (in text format) sent to the set handler of the command for the slow", &scUserData);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint64_t>(OPT_SL_COMMAND_ID, "The command identification number(cidn)", &scCmdID);
		ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_SL_COMMAND_SCHEDULE_DELAY, "The millisecond beetwen a step an the next of the scheduler[in milliseconds]", 1000000, &scSubmissionSchedulerDelay);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_SL_COMMAND_SUBMISSION_RETRY_DELAY, "The millisecond beetwen submission checker run", 1000, &scSubmissionSubmissionRetryDelay);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<bool>(OPT_SL_COMMAND_SET_FEATURES_LOCK, "if true will lock the feature to the command modification", &scFeaturesLock);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<uint32_t>(OPT_SL_COMMAND_SET_FEATURES_SCHEDULER_WAIT, "The millisecond beetwen two step of the scheduler", &scFeaturesSchedWait);
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption<string>(OPT_RT_ATTRIBUTE_VALUE, "The attribute and value for the input attribute in rt control unit [attribute:value]", &rtAttributeValue);
		
		//! [UIToolkit Attribute Init]
        
        //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
        //! [UIToolkit Init]
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_STATE)){
            op = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int>(OPT_STATE);
        }
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_CU_ID)){
            deviceID = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_CU_ID);
        }
        
        if(deviceID.size()==0) throw CException(1, "invalid device identification string", "check param");
        
        
        if(op == 5 && !ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SCHEDULE_TIME))
            throw CException(3, "The set schedule time code need the param \"stime\"", "device controller creation");
        else{
            if(op == 5){
                scheduleTime = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<long>(OPT_SCHEDULE_TIME);
            }
        }
        
        DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(deviceID, timeout);
        if(!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
        
		//get the actual state of device
        err = controller->getState(deviceState);
        if(err == ErrorCode::EC_TIMEOUT && op!=11) throw CException(5, "Time out on connection", "Get state for device");
        
        if(printState) {
            std::cout << "Current state:";
            print_state(deviceState);
            std::cout << std::endl;
        }
        
		if(printType) {
			std::string control_unit_type;
            std::cout << "Control unit type:";
            err = controller->getType(control_unit_type);
			if(err == ErrorCode::EC_TIMEOUT) throw CException(6, "Time out on connection", "Get type of the control unit");
            std::cout << control_unit_type << std::endl;
        }
		
        switch (op) {
            case 1:
                if(deviceState == CUStateKey::DEINIT) {
                    /*
                     Start the control unit
                     */
                    err = controller->initDevice();
                    if(err == ErrorCode::EC_TIMEOUT) throw CException(6, "Time out on connection", "Set device to init state");
                }else{
                    throw CException(7, "The device is not in the deinitlizied state", "Set device to init state");
                }
                break;
            case 2:
                if(deviceState == CUStateKey::INIT || deviceState == CUStateKey::STOP) {
                    /*
                     Start the control unit
                     */
                    err = controller->startDevice();
                    if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to start state");
                }else{
                    throw CException(7, "The device is not in the init or stop state", "Set device to start state");
                }
                break;
            case 3:
                if(deviceState == CUStateKey::START) {
                    /*
                     Start the control unit
                     */
                    err = controller->stopDevice();
                    if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to stop state");
                } else {
                    throw CException(8, "The device is not in the start", "Set device to stop state");
                }
                break;
            case 4:
                if(deviceState == CUStateKey::STOP || deviceState == CUStateKey::INIT) {
                    /*
                     Start the control unit
                     */
                    err = controller->deinitDevice();
                    if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
                } else {
                    throw CException(29, "Device is not in stop or init state", "Set device to start");
                }
                break;
            case 5:
                if(deviceState != CUStateKey::DEINIT) {
                    /*
                     Start the control unit
                     */
                    err = controller->setScheduleDelay(scheduleTime);
                    if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
                } else {
                    throw CException(29, "Device can't be in deinit state", "Set device schedule time");
                }
                break;
            case 6: {
				//check sc
				uint64_t command_id = 0;
				auto_ptr<CDataWrapper> userData;
				bool canBeExecuted = scAlias.size() > 0;
				canBeExecuted = canBeExecuted && (checkSubmissionRule(scSubmissionRule) != -1);
				if(canBeExecuted) {
					if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_SUBMISSION_RETRY_DELAY)) {
						std::cout << "Custom checker delay submitted -> " << scSubmissionSubmissionRetryDelay << std::endl;
					}
					
					if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_DATA)) {
						userData.reset(new CDataWrapper());
						if(userData.get())userData->setSerializedJsonData(scUserData.c_str());
						std::cout << "User data submitted" << std::endl;
						std::cout << "-----------------------------------------" << std::endl;
						std::cout << userData->getJSONString() << std::endl;
						std::cout << "-----------------------------------------" << std::endl;
					}
					err = controller->submitSlowControlCommand(scAlias,
															   static_cast<chaos_batch::SubmissionRuleType::SubmissionRule>(checkSubmissionRule(scSubmissionRule)),
															   scSubmissionPriority,
															   command_id,
															   scExecutionChannel,
															   scSubmissionSchedulerDelay,
															   scSubmissionSubmissionRetryDelay,
															   userData.get());
					if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
					std::cout << "Command submitted successfully his command idedentification number(cidn) is= " << command_id << std::endl;
				} else {
					throw CException(29, "Device can't be in deinit state", "Send slow command");
				}
			}
                break;
			case 7:{
				err = controller->killCurrentCommand();
                break;
			}
			case 8:{
				//chec if has been porvided a command ID
				if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_ID)) {
					throw CException(1, "No command id provided", "OPCODE 8");
				}
				chaos_batch::CommandState command_state;
				command_state.command_id = scCmdID;
				err = controller->getCommandState(command_state);
				std::cout << "Device state start -------------------------------------------------------" << std::endl;
				std::cout << "Command";
				switch (command_state.last_event) {
					case chaos_batch::BatchCommandEventType::EVT_COMPLETED:
						std::cout << " has completed"<< std::endl;;
						break;
					case chaos_batch::BatchCommandEventType::EVT_FAULT:
						std::cout << " has fault";
						std::cout << "Error code		:"<<command_state.fault_description.code<< std::endl;
						std::cout << "Error domain		:"<<command_state.fault_description.domain<< std::endl;
						std::cout << "Error description	:"<<command_state.fault_description.description<< std::endl;
						break;
					case chaos_batch::BatchCommandEventType::EVT_KILLED:
						std::cout << " has been killed"<< std::endl;
						break;
					case chaos_batch::BatchCommandEventType::EVT_PAUSED:
						std::cout << " has been paused"<< std::endl;
						break;
					case chaos_batch::BatchCommandEventType::EVT_QUEUED:
						std::cout << " has been queued"<< std::endl;
						break;
					case chaos_batch::BatchCommandEventType::EVT_RUNNING:
						std::cout << " is running"<< std::endl;
						break;
					case chaos_batch::BatchCommandEventType::EVT_WAITING:
						std::cout << " is waiting"<< std::endl;
						break;
				}
				std::cout << "Device state end ---------------------------------------------------------" << std::endl;
				break;
			}
			case 9: {
				//set an input attribute of the dataset(rtcu)
				if(!ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_RT_ATTRIBUTE_VALUE)) {
					throw CException(1, "No attribute and value setupped", "OPCODE 9");
				}
				if(rtAttributeValue.find(":")== string::npos) {
					throw CException(2, "Attribute param not well formet, lak of ':' character (param_name:param_value)", "OPCODE 9");
					
				}
				std::string param_name = rtAttributeValue.substr(0, rtAttributeValue.find(":"));
				std::string param_value = rtAttributeValue.substr(rtAttributeValue.find(":")+1);
				//get name and value of the attribute
				err = controller->setAttributeToValue(param_name.c_str(), param_value.c_str(), false);
				if(err == ErrorCode::EC_TIMEOUT)
					throw CException(3, "Time out on connection", "OPCODE 9");
				else if(err != ErrorCode::EC_NO_ERROR)
					throw CException(3, "Time out on connection", "OPCODE 9");
				
				break;
			}
				
			case 10:{
				err = controller->flushCommandStateHistory();
				if(err == ErrorCode::EC_TIMEOUT) throw CException(3, "Time out on connection", "OPCODE 10");
				break;
			}
				
			case 11:{
				controller->fetchCurrentDeviceValue();
				CDataWrapper * dataWrapper = controller->getLiveCDataWrapperPtr();
				if(dataWrapper) {
					std::cout << dataWrapper->getJSONString() << std::endl;
				}
				std::string str_value;
				for (int idx = 0; idx < key_to_show.size(); idx++) {
					if(!controller->getAttributeStrValue(key_to_show[idx], str_value)) {
						std::cout << key_to_show[idx] << " = " << str_value << std::endl;
					}
				}
				
				break;
			}
                
        }
        
		if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_SET_FEATURES_LOCK) ||
		   ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_SET_FEATURES_SCHEDULER_WAIT)){
			
			chaos_batch::features::Features features;
			
			//we can set the features
			if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_SET_FEATURES_LOCK)) {
				features.featuresFlag |= chaos_batch::features::FeaturesFlagTypes::FF_LOCK_USER_MOD;
				std::cout << "Set the lock feature to -> " << scFeaturesLock << std::endl;
				//set the lock
				err = controller->setSlowCommandLockOnFeatures(scFeaturesLock);
				if(err == ErrorCode::EC_TIMEOUT) throw CException(5, "Time out on connection", "setSlowCommandLockOnFeatures");
			}
			
			if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SL_COMMAND_SET_FEATURES_SCHEDULER_WAIT)) {
				features.featuresFlag |= chaos_batch::features::FeaturesFlagTypes::FF_SET_SCHEDULER_DELAY;
				features.featureSchedulerStepsDelay = scFeaturesSchedWait;
				std::cout << "Set the sched wait feature to -> " << scFeaturesSchedWait << " on execution channel " << scExecutionChannel << std::endl;
				//se the features
				err = controller->setSlowCommandFeatures(features, scFeaturesLock, scExecutionChannel);
				if(err == ErrorCode::EC_TIMEOUT) throw CException(5, "Time out on connection", "setSlowCommandFeatures");
			}
			
		}
		
        if( printState && (op>=1 && op<=4)){
            //get the actual state of device
            err = controller->getState(deviceState);
            if(err == ErrorCode::EC_TIMEOUT) throw CException(5, "Time out on connection", "Get state for device");
            std::cout << "State after operation:";
            print_state(deviceState);
            std::cout << std::endl;
        }
        
		if(controller)
			HLDataApi::getInstance()->disposeDeviceControllerPtr(controller);
		
    } catch (CException& e) {
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    try {
        //! [UIToolkit Deinit]
        ChaosUIToolkit::getInstance()->deinit();
        //! [UIToolkit Deinit]
    } catch (CException& e) {
        std::cerr << e.errorCode << " - "<< e.errorDomain << " - " << e.errorMessage << std::endl;
    }
    return 0;
}

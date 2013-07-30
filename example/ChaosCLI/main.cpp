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
namespace cccs = chaos::cu::control_manager::slow_command;

#define OPT_STATE           "op"
#define OPT_DEVICE_ID       "deviceid"
#define OPT_SCHEDULE_TIME   "stime"
#define OPT_PRINT_STATE     "print-state"
//--------------slow contorol option------------------
#define OPT_SL_ALIAS                            "sc-alias"
#define OPT_SL_PRIORITY                         "sc-priority"
#define OPT_SL_SUBMISSION_RULE                  "sc-sub-rule"
#define OPT_SL_COMMAND_DATA                     "sc-cmd-data"
#define OPT_SL_COMMAND_SCHEDULE_DELAY           "sc-cmd-sched-delay"
#define OPT_SL_COMMAND_SUBMISSION_RETRY_DELAY   "sc-cmd-submission-retry-delay"

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
    if( scSubmissionRule.compare("normal") ) {
        return cccs::SubmissionRuleType::SUBMIT_NORMAL;
    } else if( scSubmissionRule.compare("stack") ) {
        return cccs::SubmissionRuleType::SUBMIT_AND_Stack;
    } else if( scSubmissionRule.compare("kill") ) {
        return cccs::SubmissionRuleType::SUBMIT_AND_Kill;
    } else return -1;
}

int main (int argc, char* argv[] )
{
    try {
        int err = 0;
        int op =-1;
        bool printState = false;
        long scheduleTime;
        string deviceID;
        string scAlias;
        string scSubmissionRule;
        string scUserData;
        uint32_t scSubmissionPriority;
        uint32_t scSubmissionSchedulerDelay;
        uint32_t scSubmissionSubmissionRetryDelay;
        
        CDeviceNetworkAddress deviceNetworkAddress;
        CUStateKey::ControlUnitState deviceState;
       
        //! [UIToolkit Attribute Init]
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_DEVICE_ID, po::value<string>(), "The identification string of the device");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_STATE, po::value<int>()->default_value(0), "The state to set on the device{init = 1, start=2, stop=3, deinit=4, set schedule time=5, submite slow command=6}");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SCHEDULE_TIME, po::value<long>(), "the time in microseconds for devide schedule time");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_PRINT_STATE, po::value<bool>(&printState)->default_value(false), "Print the state of the device");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SL_ALIAS, po::value<string>(&scAlias)->default_value(""), "The alias associted to the command for the slow control cu");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SL_SUBMISSION_RULE, po::value<string>(&scSubmissionRule)->default_value("normal"), "The rule used for submit the command for the slow control cu [normal, stack, kill]");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SL_PRIORITY, po::value<uint32_t>(&scSubmissionPriority)->default_value(50), "The priority used for submit the command for the slow control cu");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SL_COMMAND_DATA, po::value<string>(&scUserData), "The bson pack (in text format) sent to the set handler of the command for the slow");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SL_COMMAND_SCHEDULE_DELAY, po::value<uint32_t>(&scSubmissionSchedulerDelay)->default_value(0), "The millisecond beetwen a step an the next of the scheduler[in milliseconds]");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SL_COMMAND_SUBMISSION_RETRY_DELAY, po::value<uint32_t>(&scSubmissionSubmissionRetryDelay)->default_value(0), "The millisecond beetwen submission checker run");
        //! [UIToolkit Attribute Init]
        
        //! [UIToolkit Init]
        ChaosUIToolkit::getInstance()->init(argc, argv);
        //! [UIToolkit Init]
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_STATE)){
            op = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<int>(OPT_STATE);
        }
        
        if(ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_DEVICE_ID)){
            deviceID = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<string>(OPT_DEVICE_ID);
        }
        
        if(deviceID.size()==0) throw CException(1, "invalid device identification string", "check param");
        
        
        if(op == 5 && !ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->hasOption(OPT_SCHEDULE_TIME))
            throw CException(3, "The set schedule time code need the param \"stime\"", "device controller creation");
        else{
            if(op == 5){
                scheduleTime = ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->getOption<long>(OPT_SCHEDULE_TIME);
            }
        }
        
        DeviceController *controller = HLDataApi::getInstance()->getControllerForDeviceID(deviceID);
        if(!controller) throw CException(4, "Error allcoating decive controller", "device controller creation");
        
        //get the actual state of device
        err = controller->getState(deviceState);
        if(err == ErrorCode::EC_TIMEOUT) throw CException(5, "Time out on connection", "Get state for device");
        
        if(printState) {
            std::cout << "Current state:";
            print_state(deviceState);
            std::cout << std::endl;
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
            case 6:
                if(deviceState == CUStateKey::START) {
                    //check sc
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
                        err = controller->submitSlowControlCommand(scAlias, static_cast<cccs::SubmissionRuleType::SubmissionRule>(checkSubmissionRule(scSubmissionRule)), scSubmissionPriority, scSubmissionSchedulerDelay, scSubmissionSubmissionRetryDelay, userData.get());
                        if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
                    } else {
                        throw CException(29, "Device can't be in deinit state", "Set device schedule time");
                    }
                } else {
                    throw CException(29, "Device can't be in deinit state", "Set device schedule time");
                }
                break;
                
        }
        
        if( printState && (op>=1 && op<=4)){
            //get the actual state of device
            err = controller->getState(deviceState);
            if(err == ErrorCode::EC_TIMEOUT) throw CException(5, "Time out on connection", "Get state for device");
            std::cout << "State after operation:";
            print_state(deviceState);
            std::cout << std::endl;
        }
        
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

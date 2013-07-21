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


#define OPT_STATE           "op"
#define OPT_DEVICE_ID       "deviceid"
#define OPT_SCHEDULE_TIME   "stime"
#define OPT_PRINT_STATE     "print-state"

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

int main (int argc, char* argv[] )
{
    try {
        int err = 0;
        int op =-1;
        bool printState = false;
        long scheduleTime;
        string deviceID;
        CDeviceNetworkAddress deviceNetworkAddress;
        CUStateKey::ControlUnitState deviceState;

            //! [UIToolkit Attribute Init]
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_DEVICE_ID, po::value<string>(), "The identification string of the device");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_STATE, po::value<int>()->default_value(0), "The state to set on the device{init = 1, start=2, stop=3, deinit=4, set schedule time=5}");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_SCHEDULE_TIME, po::value<long>(), "the time in microseconds for devide schedule time");
        ChaosUIToolkit::getInstance()->getGlobalConfigurationInstance()->addOption(OPT_PRINT_STATE, po::value<bool>(&printState)->default_value(false), "Print the state of the device");
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
                    err = controller->setScheduleDelay(1000000);
                    if(err == ErrorCode::EC_TIMEOUT) throw CException(2, "Time out on connection", "Set device to deinit state");
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

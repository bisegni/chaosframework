/*
 *	ChaosUIToolkit.h
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

#include "ChaosUIToolkitCWrapper.h"
#include <chaos/ui_toolkit/ChaosUIToolkit.h>
#include <chaos/ui_toolkit/LowLevelApi/LLRpcApi.h>
#include <chaos/ui_toolkit/HighLevelApi/HLDataApi.h>

using namespace chaos;
using namespace chaos::ui;

extern "C" {
    int initToolkit(const char* startupParameter) {
        int err = 0;
        istringstream optionStream;
        optionStream.str(startupParameter);
        try {
             ChaosUIToolkit::getInstance()->init(optionStream);
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    DevCtrl * getNewControllerForDeviceID(const char* deviceID) {
        DeviceController *controller = NULL;
        try{
            string dID = deviceID;
            controller = HLDataApi::getInstance()->getControllerForDeviceID(dID);
        } catch (CException& e) {
        }
        return (DevCtrl*) controller;
    }
    
    int initDevice(DevCtrl *dCtrl) {
        int err = 0;
        try{
            if(dCtrl)
                err = ((DeviceController*)dCtrl)->initDevice();
            else
                err = -1000;
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int startDevice(DevCtrl *dCtrl) {
        int err = 0;
        try{
            if(dCtrl)
                err = ((DeviceController*)dCtrl)->startDevice();
            else
                err = -1000;
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    int stopDevice(DevCtrl *dCtrl) {
        int err = 0;
        try{
            if(dCtrl)
                err = ((DeviceController*)dCtrl)->stopDevice();
            else
                err = -1000;
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int deinitDevice(DevCtrl *dCtrl) {
        int err = 0;
        try{
            if(dCtrl)
                err = ((DeviceController*)dCtrl)->deinitDevice();
            else
                err = -1000;
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
    
    int deinitToolkit() {
        int err = 0;
        try{
            ChaosUIToolkit::getInstance()->deinit();
        } catch (CException& e) {
            err = e.errorCode;
        }
        return err;
    }
}
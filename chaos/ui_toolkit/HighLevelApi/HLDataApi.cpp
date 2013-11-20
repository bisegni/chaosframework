/*	
 *	HLDataApi.cpp
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
#include "HLDataApi.h"

using namespace std;
using namespace chaos;
using namespace chaos::ui;

/*
 * Constructor
 */
HLDataApi::HLDataApi() {
    
}

/*
 * Distructor
 */
HLDataApi::~HLDataApi() {
   }

/*
 LL Rpc Api static initialization it should be called once for application
 */
void HLDataApi::init() throw (CException) {
    
}
/*
 Deinitialization of LL rpc api
 */
void HLDataApi::deinit() throw (CException) {
    for (map<string, DeviceController*>::iterator controllerIterator = controllerMap.begin(); 
         controllerIterator != controllerMap.end(); 
         controllerIterator++) {
        
        DeviceController *ctrl = controllerIterator->second;
        
        //dispose it
        delete(ctrl);
    }

}


DeviceController *HLDataApi::getControllerForDeviceID(string& deviceID, uint32_t controller_timeout) throw (CException) {
    DeviceController *deviceController = new DeviceController(deviceID);
	deviceController->setRequestTimeWaith(controller_timeout);
    deviceController->updateChannel();
    
    controllerMap.insert(make_pair(deviceID, deviceController));
    
    return deviceController;
}

void HLDataApi::disposeDeviceControllerPtr(DeviceController *ctrl) throw (CException) {
    if(!ctrl) return;
    string deviceID;
    ctrl->getDeviceId(deviceID);
    
    //remove device from the map of all active device
    controllerMap.erase(deviceID);
    
    //dispose the devie
    delete ctrl;
    ctrl = NULL;
}

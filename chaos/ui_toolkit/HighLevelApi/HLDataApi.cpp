/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */
#include "HLDataApi.h"
using namespace std;
using namespace chaos;
using namespace chaos::ui;
using namespace chaos::common::message;

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
    LDBG_<<"["<<__PRETTY_FUNCTION__<<"] deleting device controllers";
    
    for (map<string, DeviceController*>::iterator controllerIterator = controllerMap.begin();
         controllerIterator != controllerMap.end();
         controllerIterator++) {
        LDBG_<<"["<<__PRETTY_FUNCTION__<<"] deleting device controller:"<<controllerIterator->first<<" ptr:"<<(uintptr_t)std::hex<<controllerIterator->second;
        
        DeviceController *ctrl = controllerIterator->second;
        //dispose it
        delete(ctrl);
    }
    
}


DeviceController *HLDataApi::getControllerForDeviceID(string deviceID,
                                                      uint32_t controller_timeout) throw (CException) {
    
    DeviceController *deviceController = new DeviceController(deviceID);
    deviceController->setRequestTimeWaith(controller_timeout);
    deviceController->updateChannel();
    LDBG_<<"["<<__PRETTY_FUNCTION__<<"] inserting new device controller:"<<deviceID<<" ptr:"<<(uintptr_t)std::hex<<deviceController;
    controllerMap.insert(make_pair(deviceID,
                                   deviceController));
    
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

void HLDataApi::createNewSnapshot(const std::string& snapshot_name) {
    
}

void HLDataApi::deleteSnapshot(const std::string& snapshot_name) {
    
}

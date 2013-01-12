//
//  ReactorController.cpp
//  BenchTest
//
//  Created by Claudio Bisegni on 1/12/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "ReactorController.h"


ReactorController::ReactorController(string& _rName):reactorID(_rName) {
    chaosThread.reset(new chaos::CThread(this));
}

ReactorController::~ReactorController() {
    
}

void ReactorController::init() {
    state = INIT;
    chaosReactorController = chaos::ui::HLDataApi::getInstance()->getControllerForDeviceID(reactorID);
    
    chaosThread->start();
}

void ReactorController::deinit() {
    chaosThread->stop();
    chaosThread->join();
}

void ReactorController::setScheduleDelay(int64_t scheduleDaly) {
    if(chaosThread.get()) chaosThread->setDelayBeetwenTask(scheduleDaly);
}
 
void ReactorController::joinThread(){
    if(chaosThread.get()) chaosThread->join();
}
                      
void ReactorController::executeOnThread(const string&) throw(CException) {
    //state of the reactor
    int err = ErrorCode::EC_NO_ERROR;
    CUStateKey::ControlUnitState deviceState;
    switch(state) {
        case INIT:
            err = chaosReactorController->getState(deviceState);
            break;
    }
    
}


//
//  ReactorController.cpp
//  BenchTest
//
//  Created by Claudio Bisegni on 1/12/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include <chaos/common/global.h>
#include "ReactorController.h"


ReactorController::ReactorController(string& _rName, vector<double> *refVec, int refIdx, int32_t _simulationSpeed):reactorID(_rName) {
    chaosThread.reset(new chaos::CThread(this));
    chaosThread->setDelayBeetwenTask(_simulationSpeed);
    simulationSpeed = _simulationSpeed;
    //set the referement for this reactor controller
    for(int idx = 0; idx < Q; idx++) {
        rif[idx] = (*refVec)[refIdx + idx];
    }
}

ReactorController::~ReactorController() {
    
}

void ReactorController::init() {
    int err = 0;
    state = INIT;
    
    CUStateKey::ControlUnitState deviceState;
    chaosReactorController = chaos::ui::HLDataApi::getInstance()->getControllerForDeviceID(reactorID);
    CHAOS_ASSERT(chaosReactorController)
    
    //start the reactor
    err = chaosReactorController->getState(deviceState);
    if(deviceState != CUStateKey::START) {
        if(deviceState == CUStateKey::DEINIT) {
            err = chaosReactorController->initDevice();
            err = chaosReactorController->startDevice();
        } else if(deviceState == CUStateKey::STOP) {
            err = chaosReactorController->startDevice();
        }
    }
    
    chaosReactorController->setScheduleDelay(simulationSpeed);
    //start the controller
    chaosThread->start();
    cycleCount = 0;
    lastExecutionTime = steady_clock::now();

}

void ReactorController::deinit() {
    int err = 0;
    if(chaosReactorController) {
        err = chaosReactorController->stopDevice();
        err = chaosReactorController->deinitDevice();
    }
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
    cycleCount++;
    //execute the control of the reactor
    
    chaosReactorController->fetchCurrentDeviceValue();
    
    chaos::CDataWrapper *reactorCurrentState = chaosReactorController->getCurrentData();
    
    if(!reactorCurrentState) return; // no data found     //fill the array 
    Batch_Controller::y[0] = reactorCurrentState->getDoubleValue("output_a");
    Batch_Controller::y[1] = reactorCurrentState->getDoubleValue("output_b");
    
    Batch_Controller::compute_controllo();
    
    //LAPP_ << "Control_A=" << Batch_Controller::u[0] << " reactor Control_B="<< Batch_Controller::u[1];
    boost::chrono::microseconds diff = boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::steady_clock::now() - lastExecutionTime);
    if(diff.count() >= 1000000) {
        //print every second the value
        LAPP_ << "Cycle count =" << cycleCount << " micro sec passed=" << diff.count() << " Control_A=" << Batch_Controller::u[0] << " reactor Control_B="<< Batch_Controller::u[1];
        cycleCount = 0;
        lastExecutionTime = boost::chrono::steady_clock::now();
    }

    chaosReactorController->setAttributeToValue("input_a", DataType::TYPE_DOUBLE, (void*)&Batch_Controller::u[0], true);
    chaosReactorController->setAttributeToValue("input_b", DataType::TYPE_DOUBLE, (void*)&Batch_Controller::u[1], true);
}


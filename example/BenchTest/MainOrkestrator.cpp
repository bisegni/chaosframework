//
//  MainOrkestrator.cpp
//  BenchTest
//
//  Created by Claudio Bisegni on 1/13/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "MainOrkestrator.h"
#include <cstring>
#include <signal.h>
using namespace std;

chaos::WaitSemaphore MainOrkestrator::waitCloseSemaphore;

MainOrkestrator::MainOrkestrator(std::vector< std::string > *_reactordeviceIDs, std::vector< double > *_reactorsReferements, std::vector< uint64_t > *_reactorSimulatedSpeed) {
    waithUntilEnd = true;
    reactordeviceIDs = _reactordeviceIDs;
    reactorsReferements = _reactorsReferements;
    reactorSimulatedSpeed = _reactorSimulatedSpeed;
    
    if (signal((int) SIGINT, MainOrkestrator::signalHanlder) == SIG_ERR){
        LERR_ << "SIGINT Signal handler registraiton error";
    }
    
    if (signal((int) SIGINT, MainOrkestrator::signalHanlder) == SIG_ERR){
        LERR_ << "SIGINT Signal handler registraiton error";
    }
}

MainOrkestrator::~MainOrkestrator() {
    
}

void MainOrkestrator::init() {
    for(int idx = 0, refIdx = 0; idx < reactordeviceIDs->size(); idx++, refIdx+=Q) {
        ReactorController *ctrl = new ReactorController((*reactordeviceIDs)[idx], reactorsReferements, refIdx, (*reactorSimulatedSpeed)[idx]);
        if(ctrl) ctrl->init();
        reactorControllerList.add(ctrl);
    }
}

void MainOrkestrator::join() {
    waitCloseSemaphore.wait();
    
    //shutdown all controller
    for(int i=0; i < reactorControllerList.size(); i++) {
        reactorControllerList[i]->deinit();
    }
}

void MainOrkestrator::signalHanlder(int signalNumber) {
    waitCloseSemaphore.unlock();
}

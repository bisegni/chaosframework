//
//  MainOrkestrator.cpp
//  BenchTest
//
//  Created by Claudio Bisegni on 1/13/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "MainOrkestrator.h"

chaos::WaitSemaphore MainOrkestrator::waitCloseSemaphore;

MainOrkestrator::MainOrkestrator(std::vector< std::string > *_reactordeviceIDs, std::vector< double > *_reactorsReferements) {
    waithUntilEnd = true;
    reactordeviceIDs = _reactordeviceIDs;
    reactorsReferements = _reactorsReferements;
    
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
        ReactorController *ctrl = new ReactorController((*reactordeviceIDs)[idx], reactorsReferements, refIdx);
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

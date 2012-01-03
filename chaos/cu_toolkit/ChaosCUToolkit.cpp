    //
    //  ChaosCUToolkit.cpp
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 10/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "ChaosCUToolkit.h"
#include "DataManager/DataManager.h"
#include "CommandManager/CommandManager.h"
#include "ControlManager/ControlManager.h"

#include <csignal>


using namespace std;
using namespace chaos;
using boost::shared_ptr;

boost::mutex ChaosCUToolkit::monitor;
boost::condition ChaosCUToolkit::endWaithCondition;

/*
 *
 */
void ChaosCUToolkit::init(int argc, const char* argv[])  throw(CException) {
    try {
            //init common stage
        ChaosCommon::init(argc, argv);
    
        LAPP_ << "Initializing CHAOS Control System Library";
    
        if (signal((int) SIGINT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGINT Signal handler registraiton error";
        }
 
        if (signal((int) SIGQUIT, ChaosCUToolkit::signalHanlder) == SIG_ERR){
            LERR_ << "SIGQUIT Signal handler registraiton error";
        }
    

        LAPP_ << "Initialization Data Manager";
        DataManager::getInstance()->init();
        LAPP_ << "Data Manager Initialized";
        
            //start command manager, this manager must be the last to startup
        LAPP_ << "Initialization Command Manager";
        CommandManager::getInstance()->init();
        CommandManager::getInstance()->privLibControllerPtr=this;
        LAPP_ << "Command Manager Initialized";
        
            //start Control Manager
        LAPP_ << "Initialization Control Manager";
        ControlManager::getInstance()->init();
        
        LAPP_ << "Control Manager Initialized";
        
        LAPP_ << "CHAOS Control System Library Initialized";

    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
        //start data manager
    
}

/*
 *
 */ 
void ChaosCUToolkit::start(bool waithUntilEnd, bool deinitiOnEnd){
        //lock o monitor for waith the end
    lock lk(monitor);
    try {
        LAPP_ << "Starting CHAOS Control System Library";
            //start command manager, this manager must be the last to startup
        LAPP_ << "Starting Data Manager";
        DataManager::getInstance()->start();
        LAPP_ << "Data Manager Started";
        
            //start command manager, this manager must be the last to startup
        LAPP_ << "Starting Command Manager";
        CommandManager::getInstance()->start();
        LAPP_ << "Command Manager Started";
        
            //start Control Manager
        LAPP_ << "Starting Control Manager";
        ControlManager::getInstance()->start();
        LAPP_ << "Control Manager Started";
        LAPP_ << "-----------------------------------------";
        LAPP_ << "CHAOS Control System Library Started";
        LAPP_ << "-----------------------------------------";
        //at this point i must with for end signal
        if(waithUntilEnd)endWaithCondition.wait(lk);
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
        exit(1);
    }
    
        //execute the deinitialization of CU
    if(waithUntilEnd && deinitiOnEnd)deinit();
}

/*
 Stop the toolkit execution
 */
void ChaosCUToolkit::stop() {
    lock lk(monitor);
        //unlock the condition for end start method
    endWaithCondition.notify_one();
}

/*
 Deiniti all the manager
 */
void ChaosCUToolkit::deinit() {
    LAPP_ << "Stopping CHAOS Control System Library";
    
        //start Control Manager
    LAPP_ << "Stopping Control Manager";
    ControlManager::getInstance()->deinit();
    LAPP_ << "Control Manager Stopped";
    
        //start command manager, this manager must be the last to startup
    LAPP_ << "Stopping Command Manager";
    CommandManager::getInstance()->deinit();
    LAPP_ << "Command Manager Stopped";
    
        //start data manager
    LAPP_ << "Stopping Data Manager";
    DataManager::getInstance()->deinit();
    LAPP_ << "Data Manager Stopped";
    
    LAPP_ << "CHAOS Control System Library Stopped";
}

/*
 * Add a new Control Unit Class for execution
 */
void ChaosCUToolkit::addControlUnit(AbstractControlUnit *newCU) {
    //call command manager to submit Custom Control Unit
    ControlManager::getInstance()->submitControlUnit(newCU);
    
    LAPP_ << "A new Control Unit " << newCU->getCUName() << " has been submitted";
}

/*
 *
 */
void ChaosCUToolkit::signalHanlder(int signalNumber) {
    lock lk(monitor);
        //unlock the condition for end start method
    endWaithCondition.notify_one();
}

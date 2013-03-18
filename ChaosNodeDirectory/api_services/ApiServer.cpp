//
//  ApiServer.cpp
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 2/26/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#include "ApiServer.h"
#include <chaos/common/global.h>

using namespace chaos;
using namespace chaos::cnd;
using namespace chaos::cnd::api;

#define AS_LAPP_ LAPP_ << "[ApiServer] - "


#define INIT_STEP   0
#define START_STEP  1
#define STOP_STEP   1
#define DEINIT_STEP 0

/*!
 */
ApiServer::ApiServer() {
    dm = new data::DataManagment();
}

/*!
 */
ApiServer::~ApiServer() {
    if(dm) delete(dm);
}

// Initialize instance
void ApiServer::init(CDataWrapper *initData) throw(chaos::CException) {
    AS_LAPP_ << "Init";
    SetupStateManager::levelUpFrom(INIT_STEP, "Already initialized");
    
    utility::ISDInterface::initImplementation(dm, initData, "chaos::cnd::data::DataManagment", "ApiServer::init");
    
    AS_LAPP_ << "Init NetworkBroker";
    networkBroker->init();
    AS_LAPP_ << "NetworkBroker Initialized";

}

// Start the implementation
void ApiServer::start() throw(chaos::CException) {
    SetupStateManager::levelUpFrom(START_STEP, "Already started");
    
    utility::ISDInterface::startImplementation(dm, "chaos::cnd::data::DataManagment", "ApiServer::start");
    
    AS_LAPP_ << "Starting DataManagment";
    networkBroker->start();
    AS_LAPP_ << "NetworkBroker Started";

}

// Start the implementation
void ApiServer::stop() throw(chaos::CException) {
    SetupStateManager::levelDownFrom(STOP_STEP, "Already stopped");
    AS_LAPP_ << "Starting NetworkBroker";
    networkBroker->stop();
    AS_LAPP_ << "NetworkBroker Started";
    
    utility::ISDInterface::stopImplementation(dm, "chaos::cnd::data::DataManagment", "ApiServer::stop");
}

// Deinit the implementation
void ApiServer::deinit() throw(chaos::CException) {
    SetupStateManager::levelDownFrom(DEINIT_STEP, "Already deinitialized");
    AS_LAPP_ << "Starting NetworkBroker";
    networkBroker->deinit();
    AS_LAPP_ << "NetworkBroker Started";
    
    utility::ISDInterface::deinitImplementation(dm, "chaos::cnd::data::DataManagment", "ApiServer::deinit");
}

/*!
 Register a class that define some api
 */
void ApiServer::registerApi(ApiProvider *apiClass) {
    CHAOS_ASSERT(apiClass)
    //set the data managment
    apiClass->dm = dm;
    
    //register the action of the api
    networkBroker->registerAction(apiClass);
}

/*!
 Deeregister a class that define some api
 */
void ApiServer::deregisterApi(ApiProvider *apiClass) {
    CHAOS_ASSERT(apiClass)
    //deregister the action of the api
    networkBroker->deregisterAction(apiClass);
    
    apiClass->dm = NULL;

}
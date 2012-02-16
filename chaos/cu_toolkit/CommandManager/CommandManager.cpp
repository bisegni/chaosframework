    //
    //  CommandManager.cpp
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 21/02/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include <vector>
#include <iostream>

#include <chaos/common/global.h>
#include "CommandManager.h"
#include <chaos/common/cconstants.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/message/MDSMessageChannel.h>

using namespace chaos;
using namespace std;
using namespace boost;

#pragma mark Public Method
/*
 */
CommandManager::CommandManager(){
    broker = new MessageBroker();
    canUseMetadataServer = GlobalConfiguration::getInstance()->isMEtadataServerConfigured();
    if(canUseMetadataServer){
        metadataServerAddress = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    }
    
}

/*
 */
CommandManager::~CommandManager(){
        //if(broker) delete();
}

/*
 * Initzialize the datamanager
 */
void CommandManager::init() throw(CException) {
    LAPP_ << "Init Command Manager";
    broker->init();
    
    LAPP_ << "Command Manager system action registration";
    
    AbstActionDescShrPtr 
    actionDescription = DeclareAction::addActionDescritionInstance<CommandManager>(this, 
                                                                                   &CommandManager::shutdown, 
                                                                                   ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN, 
                                                                                   ChaosSystemDomainAndActionLabel::ACTION_SYSTEM_SHUTDOWN,                                                                                "Shutdown the chaos control unit library instance");
    
        //registering the comman manager action
    broker->registerAction(this);
}

/*
 * Deinitzialize the command manager
 */
void CommandManager::deinit() throw(CException) {
    LAPP_ << "Deinit Command Manager";
    LAPP_ << "Command Manager system action deregistration";
        //deregistering the action
    broker->deregisterAction(this);
    LAPP_ << "Command Manager system action deregistered";
    
    broker->deinit();
}

/*
 * Start all sub process
 */
void CommandManager::start() throw(CException) {
    LAPP_ << "Start Message Broker";
    broker->start();
    LAPP_ << "Message Broker started";
}

/*
 Configure the sandbox and all subtree of the CU
 */
CDataWrapper* CommandManager::updateConfiguration(CDataWrapper*) {
    return NULL;
}

/*
 Send message over rpc channel
 */
MDSMessageChannel *CommandManager::getMetadataserverChannel() {
    string serverHost = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    return broker->getMetadataserverMessageChannel(serverHost);
}

/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void CommandManager::registerAction(DeclareAction* declareActionClass) {
    CHAOS_ASSERT(declareActionClass)
    broker->registerAction(declareActionClass);
}

/*
 Deregister actions for a determianted domain
 */
void CommandManager::deregisterAction(DeclareAction* declareActionClass) {
    CHAOS_ASSERT(declareActionClass)
    broker->deregisterAction(declareActionClass);
}

/*
 Init the sandbox
 */
CDataWrapper* CommandManager::shutdown(CDataWrapper *actionParam, bool& detachParam) throw (CException) {
    CHAOS_ASSERT(privLibControllerPtr)
    if(privLibControllerPtr) privLibControllerPtr->stop();
    return NULL;
}

/*	
 *	CommandManager.cpp
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
#include <vector>
#include <iostream>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/configuration/GlobalConfiguration.h>
#include <chaos/common/message/MDSMessageChannel.h>
#include <chaos/common/utility/ObjectFactoryRegister.h>
#include <chaos/common/event/channel/InstrumentEventChannel.h>


using namespace std;
using namespace boost;
using namespace chaos;

using namespace chaos::common::data;

using namespace chaos::cu;


#pragma mark Public Method
/*
 */
CommandManager::CommandManager(){
    broker = new NetworkBroker();
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
void CommandManager::init(void *initParam) throw(CException) {
    LAPP_ << "Init Command Manager";
    broker->init();
    
    LAPP_ << "Command Manager system action registration";
    
    AbstActionDescShrPtr 
    actionDescription = DeclareAction::addActionDescritionInstance<CommandManager>(this, 
                                                                                   &CommandManager::shutdown, 
                                                                                   ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN, 
                                                                                   ChaosSystemDomainAndActionLabel::ACTION_SYSTEM_SHUTDOWN, "Shutdown the chaos control unit library instance");
    
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

//-----------------------
void CommandManager::stop() throw(CException) {
    LAPP_ << "Stop Message Broker";
    broker->stop();
    LAPP_ << "Stop Message Broker";
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
 Get Device event channel
 */
event::channel::InstrumentEventChannel *CommandManager::getDeviceEventChannel() {
    return static_cast<event::channel::InstrumentEventChannel*>(broker->getNewEventChannelFromType(event::EventTypeInstrument));
}

    //
void CommandManager::deleteEventChannel(event::channel::EventChannel *eventChannel) {
    broker->disposeEventChannel(eventChannel);
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

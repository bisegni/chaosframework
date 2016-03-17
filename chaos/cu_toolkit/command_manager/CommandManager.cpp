/*	
 *	CommandManager.cpp
 *	!CHAOS
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
#include <chaos/cu_toolkit/command_manager/CommandManager.h>
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
using namespace chaos::common::message;

using namespace chaos::cu::command_manager;


#pragma mark Public Method
/*
 */
CommandManager::CommandManager(){
    broker = NetworkBroker::getInstance();
    canUseMetadataServer = GlobalConfiguration::getInstance()->isMEtadataServerConfigured();
    if(canUseMetadataServer){
        metadataServerAddress = GlobalConfiguration::getInstance()->getMetadataServerAddress();
    }
    
}

/*
 */
CommandManager::~CommandManager(){
}

/*
 * Initzialize the datamanager
 */
void CommandManager::init(void *initParam) throw(CException) {
    AbstActionDescShrPtr 
    actionDescription = DeclareAction::addActionDescritionInstance<CommandManager>(this, 
                                                                                   &CommandManager::shutdown, 
                                                                                   NodeDomainAndActionRPC::RPC_DOMAIN, 
                                                                                   ChaosSystemDomainAndActionLabel::ACTION_SYSTEM_SHUTDOWN, "Shutdown the chaos control unit library instance");
    
        //registering the comman manager action
    broker->registerAction(this);
}

/*
 * Deinitzialize the command manager
 */
void CommandManager::deinit() throw(CException) {

        //deregistering the action
    broker->deregisterAction(this);
}

/*
 * Start all sub process
 */
void CommandManager::start() throw(CException) {}

//-----------------------
void CommandManager::stop() throw(CException) {}

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
    return broker->getMetadataserverMessageChannel();
}

NetworkBroker *CommandManager::getNetworkBroker() {
    return broker;
}

/*
 Send message over rpc channel
 */
void CommandManager::deleteMDSChannel(MDSMessageChannel *mds_channel) {
    return broker->disposeMessageChannel(mds_channel);
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
    CHAOS_ASSERT(server_handler)
    if(server_handler) server_handler->stop();
    return NULL;
}

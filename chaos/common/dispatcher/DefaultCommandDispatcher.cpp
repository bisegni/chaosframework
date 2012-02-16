    //
    //  DummyCommand.cpp
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 06/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "../global.h"
#include "../cconstants.h"
#include "DefaultCommandDispatcher.h"

using namespace chaos;
using namespace std;
using namespace boost;

/*
 Initialization method for output buffer
 */
void DefaultCommandDispatcher::init(CDataWrapper *initConfiguration) throw(CException) {
    LAPP_ << "Initializing Default Command Dispatcher";
    CommandDispatcher::init(initConfiguration);
    
    deinitialized = true;
    LAPP_ << "Initilized Default Command Dispatcher";
}


/*
 Deinitialization method for output buffer
 */
void DefaultCommandDispatcher::deinit() throw(CException) {
    LAPP_ << "Deinitilizing Default Command Dispatcher";
        //we need to stop all das
    map<string, shared_ptr<DomainActionsScheduler> >::iterator dasIter = dasMap.begin();
    for (; dasIter != dasMap.end(); dasIter++) {
        LAPP_ << "Deinitilizing action scheduler for domain:"<< (*dasIter).second->getManagedDomainName();
            //th einitialization is enclosed into try/catch because we need to 
            //all well cleaned
        try{
            (*dasIter).second->deinit();  
        }catch(CException& cse){
           DECODE_CHAOS_EXCEPTION(cse)
        }catch(...){
            LERR_ << "-----------Exception------------";
            LERR_ << "Unmanaged error";
            LERR_ << "-----------Exception------------";
        }
        LAPP_ << "Deinitialized action scheduler for domain:";
    }
    
    deinitialized = false;
    CommandDispatcher::deinit();
    LAPP_ << "Deinitilized Default Command Dispatcher";
}


/*
 Register actions defined by AbstractActionDescriptor instance contained in the array
 */
void DefaultCommandDispatcher::registerAction(DeclareAction *declareActionClass) {
    if(!declareActionClass) return;
    
        //register the action
    CommandDispatcher::registerAction(declareActionClass);
    
        //we need to allocate the scheduler for every registered domain that doesn't exist
    
    vector<AbstActionDescShrPtr>::iterator actDescIter = declareActionClass->getActionDescriptors().begin();
    for (; actDescIter != declareActionClass->getActionDescriptors().end(); actDescIter++) {
            //get the domain executor for this action descriptor
        string domainName = (*actDescIter)->getTypeValue(AbstractActionDescriptor::ActionDomain);
        
        if(!dasMap.count(domainName)){
            shared_ptr<DomainActionsScheduler> das(new DomainActionsScheduler(getDomainActionsFromName(domainName)));
#if DEBUG
            LDBG_ << "Allocated new  actions scheduler for domain:" << domainName;
            LDBG_ << "Init actions scheduler for domain:" << domainName;
            LDBG_ << "WE MUST THING ABOUT GET GLOBAL CONF FOR INIT DomainActionsScheduler object";
#endif
            das->init(1);
#if DEBUG
            LDBG_ << "Initialized actions scheduler for domain:" << domainName;
#endif
                //add the domain scheduler to map
            dasMap.insert(make_pair(domainName, das));
            
                //register this dispatcher into Action Scheduler
                //to permit it to manage the subcommand push
            das->setDispatcher(this);
        }
    }
    
        //initialing the scheduler
        //das->init(1);
}

/*
 Deregister actions for a determianted domain
 */
void DefaultCommandDispatcher::deregisterAction(DeclareAction *declareActionClass) {
        //call superclass method
    CommandDispatcher::deregisterAction(declareActionClass);
}

/*
 This method sub the pack received by RPC system to the execution queue accordint to the pack domain
 the multithreading push is managed by OBuffer that is the superclass of DomainActionsScheduler. This method
 will ever return an allocated object. The deallocaiton is demanded to caller
 */
CDataWrapper *DefaultCommandDispatcher::dispatchCommand(CDataWrapper *commandPack) throw(CException)  {
        //allocate new Result Pack
    CDataWrapper *resultPack = new CDataWrapper();
    try{
        
        if(!commandPack) return resultPack;
        if(!commandPack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN))
            throw CException(0, "Action Call with no action domain", "DefaultCommandDispatcher::dispatchCommand");
        
        if(!commandPack->hasKey(RpcActionDefinitionKey::CS_CMDM_ACTION_NAME))
            throw CException(1, "Action Call with no action name", "DefaultCommandDispatcher::dispatchCommand");
        
        string actionDomain = commandPack->getStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_DOMAIN);
        
            //RpcActionDefinitionKey::CS_CMDM_ACTION_NAME
        if(!dasMap.count(actionDomain)) throw CException(3, "Action Domain not registered", "DefaultCommandDispatcher::dispatchCommand");
        
            //submit the action(Thread Safe)
        dasMap[actionDomain]->push(commandPack);
        
            //tag message has submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT, 0);
    }catch(CException& cse){
            //tag message has not submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT, 1);
        
            //set error to general exception error
        resultPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, cse.errorMessage);
        resultPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_DOMAIN, cse.errorDomain);
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_CODE, cse.errorCode);
    } catch(...){
            //tag message has not submitted
        resultPack->addInt32Value(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_RESULT, 1);
            //set error to general exception error
        resultPack->addStringValue(RpcActionDefinitionKey::CS_CMDM_ACTION_SUBMISSION_ERROR_MESSAGE, "Unmanaged error");
    }
    return resultPack;
}

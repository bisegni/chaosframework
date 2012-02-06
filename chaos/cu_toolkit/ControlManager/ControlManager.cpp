    //
    //  ControlManager.cpp
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 14/06/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "../../Common/global.h"
#include "ControlManager.h"
#include "../../Common/cconstants.h"
#include "../CommandManager/CommandManager.h"
#include "../../Common/configuration/GlobalConfiguration.h"

using namespace chaos;
using namespace std;

#define INIT_DEINIT_ACTION_CU_PARAM_NAME            "cu_uuid"
#define INIT_DEINIT_ACTION_CU_PARAM_DESCRIPTION     "The name of the Control Unit subject of the operation"


#define CHECK_AND_RETURN_CU_UUID_PARAM_OR_TROW(x, y)\
string y;\
if(!x->hasKey(INIT_DEINIT_ACTION_CU_PARAM_NAME)) {\
throw CException(0, "The Control Unit identifier param has not been set", "ControlManager::initSandbox");\
}\
y=x->getStringValue(INIT_DEINIT_ACTION_CU_PARAM_NAME);

#define CHECK_CU_PRESENCE_IN_MAP_OR_TROW(x)\
if(!controlUnitInstanceMap.count(x)) {\
throw CException(0, "The Control Unit identifier is not registered", "ControlManager::initSandbox");\
}



/*
 Constructor
 */
ControlManager::ControlManager() {}

/*
 Desctructor
 */
ControlManager::~ControlManager() {
    if(selfThreadPtr) delete(selfThreadPtr);
    selfThreadPtr = 0L;
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::init() throw(CException) {
    LAPP_  << "Init Control Manager";
    CThreadExecutionTaskSPtr selfSharedPtr(this);
    
    LAPP_  << "Control Manager Thread Allocation";
    selfThreadPtr = new CThread(selfSharedPtr);
    selfThreadPtr->setDelayBeetwenTask(0);
    if(!selfThreadPtr) throw CException(0, "Control Manager Thread allocation failure", "ControlManager::init");
    
        //control manager action initialization
    LAPP_  << "Control Manager system action initialization";
    
        //init CU action
    AbstActionDescShrPtr 
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this, 
                                                                                   &ControlManager::loadControlUnit, 
                                                                                   ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN, 
                                                                                   "loadControlUnit", 
                                                                                   "Control Unit load system action");
        //add parameter for control unit name
    actionDescription->addParam(INIT_DEINIT_ACTION_CU_PARAM_NAME,
    							DataType::TYPE_STRING,
    							INIT_DEINIT_ACTION_CU_PARAM_DESCRIPTION);
    
        //deinit CU action
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this, 
                                                                                   &ControlManager::unloadControlUnit, 
                                                                                   ChaosSystemDomainAndActionLabel::SYSTEM_DOMAIN, 
                                                                                   "unloadControlUnit", 
                                                                                   "Control Unit unload system action");
        //add parameter for control unit name
    actionDescription->addParam(INIT_DEINIT_ACTION_CU_PARAM_NAME,
    							DataType::TYPE_STRING,
    							INIT_DEINIT_ACTION_CU_PARAM_DESCRIPTION);

    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this, 
                                                                                   &ControlManager::updateConfiguration, 
                                                                                   "commandManager", 
                                                                                   "updateConfiguration", 
                                                                                   "Update Command Manager Configuration");
  
        //register command manager action
    CommandManager::getInstance()->registerAction(this);
    
    LAPP_ << "Get the Metadataserver channel";
    mdsChannel = CommandManager::getInstance()->getMetadataserverChannel();
    if(mdsChannel) LAPP_ << "Metadataserver has been allocated";
    else  LAPP_ << "Metadataserver allocation failed";
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::start() throw(CException) {
    LAPP_  << "Start Control Manager";
    if(selfThreadPtr) {
        selfThreadPtr->start();
        LAPP_  << "Control Manager Thread Started";
    } else {
        LERR_  << "No Control Manager Thread found";
    }
}

/*
 Deinitialize the CU Instantiator
 */
void ControlManager::deinit() throw(CException) {
    LAPP_  << "Deinit the Control Manager";
    LAPP_  << "Control Manager system action deinitialization";
        //deregistering the action
    CommandManager::getInstance()->deregisterAction(this);
    LAPP_  << "Control Manager system action deinitialized";
    
    LAPP_  << "Trying to stop Control Manager thread";
    if(selfThreadPtr)selfThreadPtr->stop(false);
    LAPP_  << "Control Manager thread notified";
    
    lockCondition.notify_one();
    if(selfThreadPtr)selfThreadPtr->join();
    LAPP_  << "Control Manager thread stoppped";
    
    LAPP_  << "Stopping all the submitted Control Unit";
    map<string, shared_ptr<AbstractControlUnit> >::iterator cuIter = controlUnitInstanceMap.begin();
    for ( ; cuIter != controlUnitInstanceMap.end(); cuIter++ ){
        shared_ptr<AbstractControlUnit> cu = (*cuIter).second;
        LAPP_  << "Deinit Control Unit Sanbox:" << cu->getCUInstance();
        try{
            cu->_undefineActionAndDataset();
        }  catch (CException& ex) {
            if(ex.errorCode != 1){
                    //these exception need to be logged
                DECODE_CHAOS_EXCEPTION(ex);
            }
        }
        LAPP_  << "Deinitilized Control Unit Sanbox:" << cu->getCUInstance();
    }
    controlUnitInstanceMap.clear();
}



/*
 Submit a new Control unit for operation
 */
void ControlManager::submitControlUnit(AbstractControlUnit *data) throw(CException) {
    mutex::scoped_lock lock(qMutex);
    submittedCUQueue.push(data);
    lock.unlock();
    lockCondition.notify_one();
}

/*
 Thread method that work on buffer item
 */
void ControlManager::executeOnThread(const string& threadIdentification) throw(CException) {
        //initialize the Control Unit
    AbstractControlUnit *curCU = 0L;
    CDataWrapper cuActionAndDataset;
    
    
    try {
        curCU = waitAndPop();
        if(!curCU){
            return;
        }
        LAPP_  << "Got new Control Unit";
        shared_ptr<AbstractControlUnit> cuPtr(curCU);
        
        LAPP_  << "Setup Control Unit Sanbox for cu:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
        cuPtr->_defineActionAndDataset(cuActionAndDataset);
        LAPP_  << "Setup finished for Control Unit Sanbox:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
            //sendConfPackToMDS(cuPtr->defaultInternalConf.get());
        sendConfPackToMDS(cuActionAndDataset);
        LAPP_  << "Talk with MDS for cu:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
                
        LAPP_  << "Configuration pack has been sent to MDS for cu:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
            //the sandbox name now is the real CUName_CUInstance before the initSandbox method call the CUInstance is
            //randomlly defined but if a CU want to ovveride it it can dureing initSandbox call
        if(controlUnitInstanceMap.count(cuPtr->getCUInstance())) {
            LERR_  << "Duplicated control unit instance " << cuPtr->getCUInstance();
            return;
        }
        LAPP_  << "Control Unit Sanbox:" << cuPtr->getCUInstance() << " ready to work";
            //add sandbox to all map of running cu
        controlUnitInstanceMap.insert(make_pair(cuPtr->getCUInstance(), cuPtr));
        
            //check if we need to autostart and init the CU
        if(cuActionAndDataset.hasKey(CUDefinitionKey::CS_CM_CU_AUTOSTART) &&
           cuActionAndDataset.getInt32Value(CUDefinitionKey::CS_CM_CU_AUTOSTART)){
                //cuPtr->initSandbox(cuPtr->defaultInternalConf.get());
            auto_ptr<SerializationBuffer> serBuffForGlobalConf(GlobalConfiguration::getInstance()->getConfiguration()->getBSONData());
            auto_ptr<CDataWrapper> masterConfiguration(new CDataWrapper(serBuffForGlobalConf->getBufferPtr()));
            masterConfiguration->appendAllElement(cuActionAndDataset);
            
#if DEBUG
            LDBG_ << masterConfiguration->getJSONString();
#endif  
        }
    } catch (CException& ex) {
        DECODE_CHAOS_EXCEPTION(ex)
    } 
        //no
    
}

/*
 
 */
void ControlManager::sendConfPackToMDS(CDataWrapper& dataToSend) {
        // dataToSend can't be sent because it is porperty of the CU
        //so we need to copy it

    auto_ptr<SerializationBuffer> serBuf(dataToSend.getBSONData());
    CDataWrapper *mdsPack = new CDataWrapper(serBuf->getBufferPtr());
        //add action for metadata server
        //add local ip and port
    
    mdsPack->addStringValue(CUDefinitionKey::CS_CM_CU_INSTANCE_NET_ADDRESS, GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort().c_str());
    
    mdsChannel->sendControlUnitDescription(mdsPack);
    
        //CommandManager::getInstance()->sendMessageToMetadataServer(mdsPack);
}

/*
 get the last insert data
 */
AbstractControlUnit* ControlManager::waitAndPop() {
    AbstractControlUnit* data = 0L;
        //boost::mutex::scoped_lock lock(qMutex);
    boost::unique_lock<boost::mutex> lock(qMutex);
    while(submittedCUQueue.empty() && !selfThreadPtr->isStopped()) {
        lockCondition.wait(lock);
    }
    if(submittedCUQueue.empty()) return 0L;
        //get the oldest data ad copy the ahsred_ptr
    data = submittedCUQueue.front();
        //remove the oldest data
    submittedCUQueue.pop();
    
    return data;
}

/*
 check for empty buffer
 */
bool ControlManager::isEmpty() const {
    boost::mutex::scoped_lock lock(qMutex);
    return submittedCUQueue.empty();
}

/*
 Init the sandbox
 */
CDataWrapper* ControlManager::loadControlUnit(CDataWrapper*, bool&) throw (CException) {
    return NULL;
}

/*
 Deinit the sandbox
 */
CDataWrapper* ControlManager::unloadControlUnit(CDataWrapper*, bool&) throw (CException) {
    return NULL;
}

/*
 Configure the sandbox and all subtree of the CU
 */
CDataWrapper* ControlManager::updateConfiguration(CDataWrapper*, bool&) {
    return NULL;
}

    //
    //  ControlManager.cpp
    //  ControlSystemLib
    //
    //  Created by Claudio Bisegni on 14/06/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

#include "../../Common/global.h"
#include "ControlManager.h"
#include "../../Common/cconstants.h"
#include "../CommandManager/CommandManager.h"
#include "../ConfigurationManager/ConfigurationManager.h"
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
if(!sanboxMap.count(x)) {\
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
                                                                                   &ControlManager::initSandbox, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_NAME, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_INIT_CU, 
                                                                                   "Control Unit initialization system action");
        //add parameter for control unit name
    actionDescription->addParam(INIT_DEINIT_ACTION_CU_PARAM_NAME,
    							CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_TYPE_STRING,
    							"The name of the Control Unit to initialize");
    
        //deinit CU action
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this, 
                                                                                   &ControlManager::deinitSandbox, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_NAME, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_DEINIT_CU, 
                                                                                   "Control Unit initialization system action");
        //add parameter for control unit name
    actionDescription->addParam(INIT_DEINIT_ACTION_CU_PARAM_NAME,
    							CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_TYPE_STRING,
    							"The name of the Control Unit to initialize");
        //start CU action
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this, 
                                                                                   &ControlManager::startSandbox, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_NAME, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_START_CU, 
                                                                                   "Control Unit start system action");
        //add parameter for control unit name
    actionDescription->addParam(INIT_DEINIT_ACTION_CU_PARAM_NAME,
    							CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_TYPE_STRING,
    							"The name of the Control Unit to Start");
        //stop CU action
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this, 
                                                                                   &ControlManager::stopSandbox, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_NAME, 
                                                                                   chaos::CS_LIB_ACTION_DOMAIN_STOP_CU, 
                                                                                   "Control Unit stop system action");
        //add parameter for control unit name
    actionDescription->addParam(INIT_DEINIT_ACTION_CU_PARAM_NAME,
    							CommandManagerConstant::CS_CMDM_ACTION_DESC_PAR_TYPE_STRING,
    							"The name of the Control Unit to Stop");
        //register command manager action
    CommandManager::getInstance()->registerAction(this);
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
    map<string, shared_ptr<ControlUnitSandbox> >::iterator sandboxIter = sanboxMap.begin();
    for ( ; sandboxIter != sanboxMap.end(); sandboxIter++ ){
        LAPP_  << "Deinit Control Unit Sanbox:" << (*sandboxIter).second->getCUName();
        try{
            (*sandboxIter).second->deinitSandbox();
        }  catch (CException& ex) {
            if(ex.errorCode != 1){
                    //these exception need to be logged
                DECODE_CHAOS_EXCEPTION(ex);
            }
        }
        LAPP_  << "Deinitilized Control Unit Sanbox:" << (*sandboxIter).second->getCUName();
    }
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
void ControlManager::executeOnThread() throw(CException) {
        //initialize the Control Unit
    AbstractControlUnit *curCU = 0L;
    CDataWrapper cuActionAndDataset;
    
    
    try {
        curCU = waitAndPop();
        if(!curCU){
            return;
        }
        LAPP_  << "Got new Control Unit";
        shared_ptr<ControlUnitSandbox> cusb(new ControlUnitSandbox(curCU));
        
        LAPP_  << "Setup Control Unit Sanbox for cu:" << cusb->getCUName() << " with instance:" << cusb->getCUInstance();
        cusb->defineSandboxAndControlUnit(cuActionAndDataset);
        LAPP_  << "Setup finished for Control Unit Sanbox:" << cusb->getCUName() << " with instance:" << cusb->getCUInstance();
            //sendConfPackToMDS(cusb->defaultInternalConf.get());
        sendConfPackToMDS(cuActionAndDataset);
        LAPP_  << "Talk with MDS for cu:" << cusb->getCUName() << " with instance:" << cusb->getCUInstance();
                
        LAPP_  << "Configuration pack has been sent to MDS for cu:" << cusb->getCUName() << " with instance:" << cusb->getCUInstance();
            //the sandbox name now is the real CUName_CUInstance before the initSandbox method call the CUInstance is
            //randomlly defined but if a CU want to ovveride it it can dureing initSandbox call
        if(sanboxMap.count(cusb->getCUInstance())) {
            LERR_  << "Duplicated control unit instance " << cusb->getSandboxName();
            return;
        }
        LAPP_  << "Control Unit Sanbox:" << cusb->getSandboxName() << " initialized";
            //add sandbox to all map of running cu
        sanboxMap.insert(make_pair(cusb->getCUInstance(), cusb));
        
            //check if we need to autostart and init the CU
        if(cuActionAndDataset.hasKey(ControlManagerConstant::CS_CM_CU_AUTOSTART) &&
           cuActionAndDataset.getInt32Value(ControlManagerConstant::CS_CM_CU_AUTOSTART)){
                //cusb->initSandbox(cusb->defaultInternalConf.get());
            auto_ptr<SerializationBuffer> serBuffForGlobalConf(GlobalConfiguration::getInstance()->getConfiguration()->getBSONData());
            auto_ptr<CDataWrapper> masterConfiguration(new CDataWrapper(serBuffForGlobalConf->getBufferPtr()));
            masterConfiguration->appendAllElement(cuActionAndDataset);
            
            cusb->initSandbox(masterConfiguration.get());
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
    mdsPack->addStringValue(CommandManagerConstant::CS_CMDM_ACTION_DOMAIN, "system");
    mdsPack->addStringValue(CommandManagerConstant::CS_CMDM_ACTION_NAME, "registerControlUnit");
        //add local ip and port
    
    mdsPack->addStringValue(ControlManagerConstant::CS_CM_CU_INSTANCE_NET_ADDRESS, GlobalConfiguration::getInstance()->getLocalServerAddressAnBasePort().c_str());
    CommandManager::getInstance()->sendMessageToMetadataServer(mdsPack);
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
 Sandbox initialization system action
 */
CDataWrapper* ControlManager::initSandbox(CDataWrapper *actionParam) throw (CException) {
    CHECK_AND_RETURN_CU_UUID_PARAM_OR_TROW(actionParam, cuUUID)
    CHECK_CU_PRESENCE_IN_MAP_OR_TROW(cuUUID)
    shared_ptr<ControlUnitSandbox> cusb = sanboxMap[cuUUID];
    LAPP_  << "Init Control Unit Sanbox for cu:" << cusb->getCUName() << " with instance:" << cusb->getCUInstance();
    
    auto_ptr<SerializationBuffer> serBuffForGlobalConf(GlobalConfiguration::getInstance()->getConfiguration()->getBSONData());
    auto_ptr<CDataWrapper> masterConfiguration(new CDataWrapper(serBuffForGlobalConf->getBufferPtr()));
    masterConfiguration->appendAllElement(*cusb->getInternalCUConfiguration());
    
    cusb->initSandbox(masterConfiguration.get());
    LAPP_  << "Initialized Control Unit Sanbox:" << cusb->getCUName() << " with instance:" << cusb->getCUInstance();

        //sanboxMap[cuUUID]->initSandbox(actionParam);
    return NULL;
}

/*
 Stop the sandbox
 */
CDataWrapper* ControlManager::deinitSandbox(CDataWrapper *actionParam) throw (CException) {
    CHECK_AND_RETURN_CU_UUID_PARAM_OR_TROW(actionParam, cuUUID)
    CHECK_CU_PRESENCE_IN_MAP_OR_TROW(cuUUID)
    sanboxMap[cuUUID]->deinitSandbox();
    return NULL;
}

/*
 Start the sandbox
 */
CDataWrapper* ControlManager::startSandbox(CDataWrapper *actionParam) throw (CException) {
    CHECK_AND_RETURN_CU_UUID_PARAM_OR_TROW(actionParam, cuUUID)
    CHECK_CU_PRESENCE_IN_MAP_OR_TROW(cuUUID)
    sanboxMap[cuUUID]->startSandbox(actionParam);
    return NULL;
}

/*
 Stop the sandbox
 */
CDataWrapper* ControlManager::stopSandbox(CDataWrapper *actionParam) throw (CException) {
    CHECK_AND_RETURN_CU_UUID_PARAM_OR_TROW(actionParam, cuUUID)
    CHECK_CU_PRESENCE_IN_MAP_OR_TROW(cuUUID)
    sanboxMap[cuUUID]->stopSandbox(actionParam);
    return NULL;
}

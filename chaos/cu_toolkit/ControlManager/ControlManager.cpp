/*	
 *	ControlManager.cpp
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
#include "../../common/global.h"
#include "ControlManager.h"
#include "../../common/cconstants.h"
#include "../CommandManager/CommandManager.h"
#include "../../common/configuration/GlobalConfiguration.h"

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

#define LCMAPP_ LAPP_ << "[Control Manager] - "
#define LCMERR_ LERR_ << "[Control Manager Error] - "

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
    LCMAPP_  << "Inititializing";
    CThreadExecutionTaskSPtr selfSharedPtr(this);
    
    LCMAPP_  << "Thread Allocation";
    selfThreadPtr = new CThread(selfSharedPtr);
    selfThreadPtr->setDelayBeetwenTask(100000);
    if(!selfThreadPtr) throw CException(0, "Thread allocation failure", "ControlManager::init");
    
    //control manager action initialization
    LCMAPP_  << "system action initialization";
    
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
    
    LCMAPP_ << "Get the Metadataserver channel";
    mdsChannel = CommandManager::getInstance()->getMetadataserverChannel();
    if(mdsChannel) LCMAPP_ << "Metadataserver has been allocated";
    else  LCMAPP_ << "Metadataserver allocation failed";
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::start() throw(CException) {
    LCMAPP_  << "Start Control Manager";
    if(selfThreadPtr) {
        selfThreadPtr->start();
        LCMAPP_  << "Thread Started";
    } else {
        LCMERR_  << "No Thread found";
    }
}

/*
 Deinitialize the CU Instantiator
 */
void ControlManager::deinit() throw(CException) {
    bool detachFake = false;
    vector<string> allCUDeviceIDToStop;
    LCMAPP_  << "Deinit the Control Manager";
    LCMAPP_  << "system action deinitialization";
    //deregistering the action
    CommandManager::getInstance()->deregisterAction(this);
    LCMAPP_  << "system action deinitialized";
    
    LCMAPP_  << "Trying to stop thread";
    if(selfThreadPtr)selfThreadPtr->stop(false);
    LCMAPP_  << "thread notified";
    
    lockCondition.notify_one();
    if(selfThreadPtr)selfThreadPtr->join();
    LCMAPP_  << "thread stoppped";
    
    LCMAPP_  << "Deinit all the submitted Control Unit";
    map<string, shared_ptr<AbstractControlUnit> >::iterator cuIter = controlUnitInstanceMap.begin();
    for ( ; cuIter != controlUnitInstanceMap.end(); cuIter++ ){
        shared_ptr<AbstractControlUnit> cu = (*cuIter).second;
        LCMAPP_  << "Deinit Control Unit. " << cu->getCUInstance();
            //load all device id for this cu
        cu->getAllDeviceId(allCUDeviceIDToStop);
        
        for (vector<string>::iterator iter =  allCUDeviceIDToStop.begin(); 
             iter != allCUDeviceIDToStop.end(); 
             iter++) {
            
                //stop all itnerna device
           
            CDataWrapper fakeDWForDeinit;
            fakeDWForDeinit.addStringValue(DatasetDefinitionkey::CS_CM_DATASET_DEVICE_ID, *iter);
            
            try{
                LCMAPP_  << "Stopping deviceid:" << *iter;
                cu->_stop(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
                        //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            
            try{
                LCMAPP_  << "Deinit deviceid:" << *iter;
                cu->_deinit(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
                        //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            try{
                LCMAPP_  << "Undefine Action And Dataset for deviceid:" << *iter;
                cu->_undefineActionAndDataset();
            }  catch (CException& ex) {
                if(ex.errorCode != 1){
                        //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
        }
       
        LCMAPP_  << "Deinitilized Control Unit Sanbox:" << cu->getCUInstance();
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
        LCMAPP_  << "Got new Control Unit";
        shared_ptr<AbstractControlUnit> cuPtr(curCU);
        
        LCMAPP_  << "Setup Control Unit Sanbox for cu:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
        cuPtr->_defineActionAndDataset(cuActionAndDataset);
        LCMAPP_  << "Setup finished for Control Unit Sanbox:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
        //sendConfPackToMDS(cuPtr->defaultInternalConf.get());
        sendConfPackToMDS(cuActionAndDataset);
        LCMAPP_  << "Talk with MDS for cu:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
        
        LCMAPP_  << "Configuration pack has been sent to MDS for cu:" << cuPtr->getCUName() << " with instance:" << cuPtr->getCUInstance();
        //the sandbox name now is the real CUName_CUInstance before the initSandbox method call the CUInstance is
        //randomlly defined but if a CU want to ovveride it it can dureing initSandbox call
        if(controlUnitInstanceMap.count(cuPtr->getCUInstance())) {
            LCMERR_  << "Duplicated control unit instance " << cuPtr->getCUInstance();
            return;
        }
        LCMAPP_  << "Control Unit Sanbox:" << cuPtr->getCUInstance() << " ready to work";
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

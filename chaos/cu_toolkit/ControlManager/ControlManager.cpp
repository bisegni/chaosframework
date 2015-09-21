/*
 *	ControlManager.cpp
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

#include <chaos/common/global.h>
#include <chaos/common/chaos_constants.h>
#include <chaos/common/healt_system/HealtManager.h>
#include <chaos/cu_toolkit/ControlManager/ControlManager.h>
#include <chaos/cu_toolkit/CommandManager/CommandManager.h>
#include <chaos/common/configuration/GlobalConfiguration.h>

#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <fstream>
#include <streambuf>

#define LCMAPP_ LAPP_ << "[Control Manager] - "
#define LCMDBG_ LDBG_ << "[Control Manager] - "
#define LCMERR_ LERR_ << "[Control Manager]"<<__LINE__<<" - "
#define WAITH_TIME_FOR_CU_REGISTRATION 2000000
#define WU_IDENTIFICATION(x) std::string(x->getCUInstance()) + std::string("-") + std::string(x->getCUID())


namespace fs = boost::filesystem;
namespace chaos_data = chaos::common::data;
namespace chaos_async = chaos::common::async_central;
namespace cu_driver_manager = chaos::cu::driver_manager;

using namespace chaos;
using namespace chaos::common::utility;
using namespace chaos::common::healt_system;
using namespace chaos::cu::command_manager;
using namespace chaos::cu::control_manager;
using namespace std;

/*
 Constructor
 */
ControlManager::ControlManager() {}

/*
 Desctructor
 */
ControlManager::~ControlManager() {
}


/*
 Initialize the CU Instantiator
 */
void ControlManager::init(void *initParameter) throw(CException) {
    //control manager action initialization
    
    AbstActionDescShrPtr actionDescription;
    use_unit_server =	GlobalConfiguration::getInstance()->hasOption(CONTROL_MANAGER_UNIT_SERVER_ENABLE) &&
    GlobalConfiguration::getInstance()->getOption<bool>(CONTROL_MANAGER_UNIT_SERVER_ENABLE);
    
    LCMAPP_ << "Get the Metadataserver channel";
    mds_channel = CommandManager::getInstance()->getMetadataserverChannel();
    if(mds_channel) LCMAPP_ << "Metadataserver has been allocated";
    else throw CException(-2, "Error allcoating metadata server channel", __PRETTY_FUNCTION__);
    
    if(use_unit_server) {
        LCMAPP_  << "Enable unit server";
        
        if(!GlobalConfiguration::getInstance()->hasOption(CONTROL_MANAGER_UNIT_SERVER_ALIAS)) {
            throw CException(-1, "No server alias param found", __PRETTY_FUNCTION__);
        }
        
        if(GlobalConfiguration::getInstance()->hasOption(CONTROL_MANAGER_UNIT_SERVER_KEY)) {
            //a key file need to be used to publish the server
            fs::path key_file_path(GlobalConfiguration::getInstance()->getOption<string>(CONTROL_MANAGER_UNIT_SERVER_KEY));
            if(fs::exists(key_file_path)) {
                if(!fs::is_directory(key_file_path)) {
                    std::ifstream key_file_stream(key_file_path.c_str());
                    unit_server_key.assign((std::istreambuf_iterator<char>(key_file_stream)), std::istreambuf_iterator<char>());
                    LCMAPP_ << "UUnit server ublic key----------------------------------------------";
                    LCMAPP_ << unit_server_key;
                    LCMAPP_ << "UUnit server ublic key----------------------------------------------";
                } else {
                    LCMERR_ << "Key file is a diretory";
                }
            } else {
                LCMERR_ << "Key file not found";
            }
            
            
        }
        
        unit_server_alias = GlobalConfiguration::getInstance()->getOption<std::string>(CONTROL_MANAGER_UNIT_SERVER_ALIAS);
        
        //init CU action
        actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                       &ControlManager::loadControlUnit,
                                                                                       UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                       UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_LOAD_CONTROL_UNIT,
                                                                                       "Control Unit load system action");
        //add parameter for control unit name
        actionDescription->addParam(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE,
                                    DataType::TYPE_STRING,
                                    "The type of the control unit to load");
        //add parameter for driver initialization string
        actionDescription->addParam(UnitServerNodeDomainAndActionRPC::PARAM_CU_LOAD_DRIVER_PARAMS,
                                    DataType::TYPE_STRING,
                                    "Driver params (pipe separated '|') to pass to the control unit instance");
        
        //deinit CU action
        actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                       &ControlManager::unloadControlUnit,
                                                                                       UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                       UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_UNLOAD_CONTROL_UNIT,
                                                                                       "Control Unit unload system action");
        //add parameter for control unit name
        actionDescription->addParam(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE,
                                    DataType::TYPE_STRING,
                                    "Alias of the control unit to unload");
        actionDescription->addParam(NodeDefinitionKey::NODE_UNIQUE_ID,
                                    DataType::TYPE_STRING,
                                    "The unique id of the control unit to unload");
        
        actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                       &ControlManager::unitServerRegistrationACK,
                                                                                       UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                       UnitServerNodeDomainAndActionRPC::ACTION_UNIT_SERVER_REG_ACK,
                                                                                       "Unit server registration ack message");
    }
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                   &ControlManager::unitServerStatus,
                                                                                   NodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                   NodeDomainAndActionRPC::ACTION_NODE_STATUS,
                                                                                   "Unit server states");
    
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                   &ControlManager::updateConfiguration,
                                                                                   "commandManager",
                                                                                   NodeDomainAndActionRPC::ACTION_UPDATE_PROPERTY,
                                                                                   "Update Command Manager Configuration");
    actionDescription = DeclareAction::addActionDescritionInstance<ControlManager>(this,
                                                                                   &ControlManager::workUnitRegistrationACK,
                                                                                   UnitServerNodeDomainAndActionRPC::RPC_DOMAIN,
                                                                                   UnitServerNodeDomainAndActionRPC::ACTION_REGISTRATION_CONTROL_UNIT_ACK,
                                                                                   "Update Command Manager Configuration");
    //register command manager action
    CommandManager::getInstance()->registerAction(this);
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::start() throw(CException) {
    LCMAPP_  << "Start cu scan timer";
    int err = 0;
    if(use_unit_server){
        //register unit server node
        HealtManager::getInstance()->addNewNode(unit_server_alias);
        HealtManager::getInstance()->addNodeMetricValue(unit_server_alias,
                                                        NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                        NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOADING,
                                                        true);
        //add unit server registration managment timer
        if((err = chaos_async::AsyncCentralManager::getInstance()->addTimer(this, 0, GlobalConfiguration::getInstance()->getOption<uint64_t>(CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC)))){
            throw chaos::CException(-1, "Error registering the Control managet timer", __PRETTY_FUNCTION__);
        }
    } else {
        startControlUnitSMThread();
    }
}

// start control units state machine thread
void ControlManager::startControlUnitSMThread() {
    thread_run = true;
    thread_registration.reset(new boost::thread(boost::bind(&ControlManager::manageControlUnit, this)));
}

// stop control unit state machien thread
void ControlManager::stopControlUnitSMThread(bool whait) {
    thread_run = false;
    thread_waith_semaphore.unlock();
    if(thread_registration.get() && whait) thread_registration->join();
}

/*
 Initialize the CU Instantiator
 */
void ControlManager::stop() throw(CException) {
    LCMAPP_  << "Stop cu scan timer";
    stopControlUnitSMThread();
}

/*
 Deinitialize the CU Instantiator
 */
void ControlManager::deinit() throw(CException) {
    bool detachFake = false;
    std::vector<const chaos::DeclareAction * > cuDeclareActionsInstance;
    vector<string> allCUDeviceIDToStop;
    LCMAPP_  << "Deinit the Control Manager";
    LCMAPP_  << "system action deinitialization";
    //deregistering the action
    CommandManager::getInstance()->deregisterAction(this);
    LCMAPP_  << "system action deinitialized";
    
    
    LCMAPP_  << "Deinit all the submitted Control Unit";
    
    for (map<string, shared_ptr<WorkUnitManagement> >::iterator cuIter = map_cuid_registered_instance.begin();
         cuIter != map_cuid_registered_instance.end();
         cuIter++ ){
        shared_ptr<WorkUnitManagement> cu = (*cuIter).second;
        
        LCMAPP_  << "Deregister RPC action for cu whith instance:" << WU_IDENTIFICATION(cu->work_unit_instance);
        cu->work_unit_instance->_getDeclareActionInstance(cuDeclareActionsInstance);
        for(int idx = 0; idx < cuDeclareActionsInstance.size(); idx++) {
            CommandManager::getInstance()->deregisterAction((chaos::DeclareAction *)cuDeclareActionsInstance[idx]);
        }
        
        //load all device id for this cu
        allCUDeviceIDToStop.push_back(cu->work_unit_instance->getDeviceID());
        
        for (vector<string>::iterator iter =  allCUDeviceIDToStop.begin();
             iter != allCUDeviceIDToStop.end();
             iter++) {
            
            //stop all itnerna device
            
            CDataWrapper fakeDWForDeinit;
            fakeDWForDeinit.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, *iter);
            try{
                LCMAPP_  << "Stopping Control Unit: " << WU_IDENTIFICATION(cu->work_unit_instance);
                cu->work_unit_instance->_stop(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
                    //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            
            try{
                LCMAPP_  << "Deiniting Control Unit: " << WU_IDENTIFICATION(cu->work_unit_instance);
                cu->work_unit_instance->_deinit(&fakeDWForDeinit, detachFake);
            }catch (CException& ex) {
                if(ex.errorCode != 1){
                    //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
            try{
                LCMAPP_  << "Undefine Action And Dataset for  Control Unit: " << WU_IDENTIFICATION(cu->work_unit_instance);
                cu->work_unit_instance->_undefineActionAndDataset();
            }  catch (CException& ex) {
                if(ex.errorCode != 1){
                    //these exception need to be logged
                    DECODE_CHAOS_EXCEPTION(ex);
                }
            }
        }
        LCMAPP_  << "Dispose event channel for Control Unit Sanbox:" << WU_IDENTIFICATION(cu->work_unit_instance);
        cuDeclareActionsInstance.clear();
        LCMAPP_  << "Unload" << cu->work_unit_instance->getCUInstance();
    }
    map_cuid_registered_instance.clear();
    map_cuid_reg_unreg_instance.clear();
    
    if(mds_channel) {
        CommandManager::getInstance()->deleteMDSChannel(mds_channel);
        mds_channel = NULL;
    }
}



/*
 Submit a new Control unit for operation
 */
void ControlManager::submitControlUnit(AbstractControlUnit *data) throw(CException) {
    //lock the hastable of cu instance and managmer
    boost::unique_lock<boost::shared_mutex> lock(mutex_queue_submitted_cu);
    queue_submitted_cu.push(data);
    
    //unlock thread
    thread_waith_semaphore.unlock();
}

void ControlManager::migrateStableAndUnstableSMCUInstance() {
    //shared access for the read of map
    //get the upgradeable lock
    UpgradeableLock registering_lock(mutex_map_cuid_reg_unreg_instance);
    UpgradeableLock registered_lock(mutex_map_cuid_registered_instance);
    
    //used the Mark Ransom Technique for avoid the temporary iterator
    //http://stackoverflow.com/questions/180516/how-to-filter-items-from-a-stdmap/180616#180616
    for (map<string, shared_ptr<WorkUnitManagement> >::iterator i = map_cuid_reg_unreg_instance.begin();
         i != map_cuid_reg_unreg_instance.end();) {
        
        if(!i->second->smNeedToSchedule()) {
            UpgradeReadToWriteLock registering_wlock(registering_lock);
            UpgradeReadToWriteLock registered_wlock(registered_lock);
            
            //now i can write on the two map
            switch (i->second->getCurrentState()) {
                case UnitStatePublishingFailure:
                    LCMAPP_<< i->second->work_unit_instance->getCUID() << " instance is erased because is in publishing failure state";
                    HealtManager::getInstance()->removeNode(i->second->work_unit_instance->getCUID());
                    break;
                case UnitStateUnpublished:
                    LCMAPP_<< i->second->work_unit_instance->getCUID() << " instance is erased becase has been successfully unpublished";
                    HealtManager::getInstance()->removeNode(i->second->work_unit_instance->getCUID());
                    break;
                case UnitStatePublished:
                    LCMAPP_<< i->second->work_unit_instance->getCUID() << " instance has been successfully registered ("<<i->first<<", "<< i->second<<")";
                    map_cuid_registered_instance.insert(make_pair(i->first, i->second));
                    break;
                default:
                    break;
            }
            
            //remove the iterator
            map_cuid_reg_unreg_instance.erase(i++); // first is executed the uncrement that return
            // a copy of the original iterator
        } else {
            ++i; //more efficient because doesn't make a copy
        }
    }
}

//! Make one steps in SM for all registring state machine
void ControlManager::makeSMSteps() {
    //lock for read the registering map
    ReadLock read_registering_lock(mutex_map_cuid_reg_unreg_instance);
    
    for (map<string, shared_ptr<WorkUnitManagement> >::iterator i = map_cuid_reg_unreg_instance.begin();
         i != map_cuid_reg_unreg_instance.end();
         i++ ){
        //make step
        try {
            if(i->second->smNeedToSchedule()) i->second->scheduleSM();
        } catch(chaos::CException& e) {
            DECODE_CHAOS_EXCEPTION(e)
        } catch(...) {
            LCMERR_ << "Undefined error";
        }
    }
}

void ControlManager::manageControlUnit() {
    //initialize the Control Unit
    
    boost::unique_lock<boost::shared_mutex> lock(mutex_queue_submitted_cu, boost::defer_lock);
    while(thread_run) {
        //lock queue
        lock.lock();
        
        //try to consume all the submitted control unit instance (after the lock no other thread can submit new on)
        while(!queue_submitted_cu.empty()) {
            //we have new instance to manage
            shared_ptr<WorkUnitManagement> wui(new WorkUnitManagement(queue_submitted_cu.front()));
            LCMAPP_  << "We have a new control unit instance:" << WU_IDENTIFICATION(wui->work_unit_instance);
            
            //remove the oldest data
            queue_submitted_cu.pop();
            
            //give a beat to the state machine
            wui->scheduleSM();
            
            //activate the sm for register the control unit instance
            wui->turnOn();
            LCMAPP_  << "Create manager for new control unit:" << WU_IDENTIFICATION(wui->work_unit_instance);
            
            //! lock the hastable
            ReadLock read_registering_lock(mutex_map_cuid_reg_unreg_instance);
            ReadLock read_registered_lock(mutex_map_cuid_registered_instance);
            
            // we can't have two different work unit with the same unique identifier within the same process
            if(map_cuid_reg_unreg_instance.count(wui->work_unit_instance->getCUID()) ||
               map_cuid_registered_instance.count(wui->work_unit_instance->getCUID())) {
                LCMERR_  << "Duplicated control unit instance " << WU_IDENTIFICATION(wui->work_unit_instance);
                continue;
            }
            LCMDBG_  << "Added to registering map" << WU_IDENTIFICATION(wui->work_unit_instance);
            
            //now we can proceed, add the network broker instance to the managment class of the work unit
            wui->mds_channel = mds_channel;
            
            //add sandbox to all map of running cu
            map_cuid_reg_unreg_instance.insert(make_pair(wui->work_unit_instance->getCUID(), wui));
        }
        lock.unlock();
        
        //migrate stable <-> unstable
        migrateStableAndUnstableSMCUInstance();
        
        //! lock the registering (unstable sm) hastable
        ReadLock read_registering_lock(mutex_map_cuid_reg_unreg_instance);
        //schedule unstable state machine steps
        if(map_cuid_reg_unreg_instance.size()) {
            //whe have control unit isntance with unstable state machine
            makeSMSteps();
            //waith some time to retry the state machine
            thread_waith_semaphore.wait(2000);
        } else {
            //we don'need to do anything else
            thread_waith_semaphore.wait();
        }
    }
}

#define CDW_HAS_KEY(x) message_data->hasKey(x)
#define CDW_STR_KEY(x) CDW_HAS_KEY(x)?message_data->getStringValue(x):""

//! message for load operation
CDataWrapper* ControlManager::loadControlUnit(CDataWrapper *message_data, bool& detach) throw (CException) {
    //check param
    IN_ACTION_PARAM_CHECK(!message_data, -1, "No param found")
    IN_ACTION_PARAM_CHECK(!message_data->hasKey(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE), -2, "No instancer alias")
    IN_ACTION_PARAM_CHECK(!message_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID), -3, "No id for the work unit instance found")
    
    std::string work_unit_type = message_data->getStringValue(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE);
    LCMAPP_ << "Get new request for instance the work unit with alias:" << work_unit_type;
    
    WriteLock write_instancer_lock(mutex_map_cu_instancer);
    IN_ACTION_PARAM_CHECK(!map_cu_alias_instancer.count(work_unit_type), -2, "No work unit instancer's found for the alias")
    
    std::string work_unit_id = message_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    std::string load_options = CDW_STR_KEY(ControlUnitNodeDefinitionKey::CONTROL_UNIT_LOAD_PARAM);
    
    //check if cuid is already present
    ReadLock read_registering_lock(mutex_map_cuid_reg_unreg_instance);
    ReadLock read_registered_lock(mutex_map_cuid_registered_instance);
    
    // we can't have two different work unit with the same unique identifier within the same process
    IN_ACTION_PARAM_CHECK(map_cuid_reg_unreg_instance.count(work_unit_id), -3, "Another work unit use the same id")
    IN_ACTION_PARAM_CHECK(map_cuid_registered_instance.count(work_unit_id), -4, "Another work unit use the same id")
    
    LCMDBG_ << "instantiate work unit ->" << "device_id:" <<work_unit_id<< " load_options:"<< load_options;
    //scan all the driver description forwarded
    std::vector<cu_driver_manager::driver::DrvRequestInfo> driver_params;
    if(message_data->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION)) {
        LCMDBG_ << "Driver param has been supplied";
        boost::scoped_ptr<CMultiTypeDataArrayWrapper> driver_descriptions(message_data->getVectorValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION));
        //scan all the driver description
        LCMDBG_ << "scan " << driver_descriptions->size() << " driver descriptions";
        for( int idx = 0; idx < driver_descriptions->size(); idx++) {
            LCMDBG_ << "scan " << idx << " driver";
            boost::scoped_ptr<CDataWrapper> driver_desc(driver_descriptions->getCDataWrapperElementAtIndex(idx));
            IN_ACTION_PARAM_CHECK(!driver_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_NAME), -4, "No driver name found")
            IN_ACTION_PARAM_CHECK(!driver_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_VERSION), -5, "No driver version found")
            IN_ACTION_PARAM_CHECK(!driver_desc->hasKey(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_INIT_PARAMETER), -6, "No driver init param name found")
            LCMDBG_ << "scan " << idx << " driver";
            cu_driver_manager::driver::DrvRequestInfo drv = {driver_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_NAME),
                driver_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_VERSION),
                driver_desc->getStringValue(ControlUnitNodeDefinitionKey::CONTROL_UNIT_DRIVER_DESCRIPTION_INIT_PARAMETER)};
            LCMDBG_ << "adding driver  " << drv.alias << "["<<drv.version << "-" << drv.init_parameter<<"]";
            driver_params.push_back(drv);
        }
    }
    
    //submit new instance of the requested control unit
    AbstractControlUnit *instance = map_cu_alias_instancer[work_unit_type]->getInstance(work_unit_id, load_options, driver_params);
    IN_ACTION_PARAM_CHECK(instance==NULL, -7, "Error creating work unit instance")
    
    //add healt metric for newly create control unit instance
    HealtManager::getInstance()->addNewNode(work_unit_id);

    //tag control uinit for mds managed
    instance->control_key = "mds";
    
    //submit contorl unit to state machine scheduling thread
    submitControlUnit(instance);
    return NULL;
}

//! message for unload operation
CDataWrapper* ControlManager::unloadControlUnit(CDataWrapper *message_data, bool& detach) throw (CException) {
    IN_ACTION_PARAM_CHECK(!message_data, -1, "No param found")
    //IN_ACTION_PARAM_CHECK(!message_data->hasKey(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE), -2, "No instancer alias")
    IN_ACTION_PARAM_CHECK(!message_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID), -2, "No id for the work unit instance found")
    
    //std::string work_unit_type = message_data->getStringValue(UnitServerNodeDomainAndActionRPC::PARAM_CONTROL_UNIT_TYPE);
    std::string work_unit_id = message_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    LCMAPP_ << "Unload operation for: " << work_unit_id;// << " of type "<<work_unit_type;
    WriteLock write_instancer_lock(mutex_map_cuid_registered_instance);
    IN_ACTION_PARAM_CHECK(!map_cuid_registered_instance.count(work_unit_id), -3, "Work unit not found on registered's map")
    
    //get the iterator for the work unit managment class
    map<string, shared_ptr<WorkUnitManagement> >::iterator iter = map_cuid_registered_instance.find(work_unit_id);
    
    //migrate the workunit into the map for registering and unregistering instance
    map_cuid_reg_unreg_instance.insert(make_pair(work_unit_id, map_cuid_registered_instance[work_unit_id]));
    
    //turn of the instance
    iter->second->turnOFF();
    map_cuid_registered_instance.erase(iter);
    
    //unlock the thread
    thread_waith_semaphore.unlock();
    return NULL;
}

CDataWrapper* ControlManager::unitServerStatus(CDataWrapper *message_data, bool &detach) throw (CException) {
    chaos_data::CDataWrapper unit_server_status;
    unit_server_status.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, unit_server_alias.size()?unit_server_alias:"No Server Defined");
    unit_server_status.addInt32Value(NodeDefinitionKey::NODE_TIMESTAMP,  (uint32_t) TimingUtil::getTimeStamp());
    LCMDBG_ << "[Action] Get Unit State";
    
    map<string, shared_ptr<WorkUnitManagement> >::iterator iter;
    for(iter = map_cuid_registered_instance.begin();iter!=map_cuid_registered_instance.end();iter++){
        chaos_data::CDataWrapper item;
        std::string cusm_state_key = iter->first+"_sm_state";
        std::string cu_state_key = iter->first+"_state";
        item.addStringValue("device_key", iter->first);
        item.addInt32Value(cusm_state_key.c_str(), (uint32_t)iter->second->getCurrentState());
        item.addInt32Value(cu_state_key.c_str(), (uint32_t)iter->second->work_unit_instance->getServiceState());
        LCMDBG_ << "[Action] Get CU managment state, \""<<iter->first<<"\" ="<<iter->second->getCurrentState();
        LCMDBG_ << "[Action] Get CU state, \""<<iter->first<<"\" ="<<(uint32_t)iter->second->work_unit_instance->getServiceState();
        unit_server_status.appendCDataWrapperToArray(item);
    }
    unit_server_status.finalizeArrayForKey(UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CU_STATES);
    mds_channel->sendUnitServerCUStates(unit_server_status);
    
    return NULL;
}

//! ack received for the registration of the uwork unit
CDataWrapper* ControlManager::workUnitRegistrationACK(CDataWrapper *message_data, bool &detach) throw (CException) {
    IN_ACTION_PARAM_CHECK(!message_data, -1, "No param found")
    IN_ACTION_PARAM_CHECK(!message_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID), -2, "No device id found")
    
    //lock the registering control unit map
    ReadLock read_registering_lock(mutex_map_cuid_reg_unreg_instance);
    std::string device_id = message_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    
    IN_ACTION_PARAM_CHECK(!map_cuid_reg_unreg_instance.count(device_id), -3, "No registering work unit found with the device id:"+device_id)
    
    //we can process the ack into the right manager
    map_cuid_reg_unreg_instance[device_id]->manageACKPack(*message_data);
    return NULL;
}

/*
 Configure the sandbox and all subtree of the CU
 */
CDataWrapper* ControlManager::updateConfiguration(CDataWrapper *message_data, bool& detach) {
    return NULL;
}

//---------------unit server state machine managment handler
void ControlManager::timeout() {
    boost::unique_lock<boost::shared_mutex> lock_sm(unit_server_sm_mutex);
    switch (unit_server_sm.current_state()[0]) {
            //Unpublished
        case 0:
            LCMDBG_ << "[Unpublished] Send first registration pack to mds";
            if(use_unit_server) {
                if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypePublishing()) == boost::msm::back::HANDLED_TRUE){
                    //gone to publishing
                    sendUnitServerRegistration();
                } else {
                    LCMERR_ << "[Unpublished] i can't be here";
                }
            } else {
                LCMDBG_ << "[Publishing] Unit server registration not sucessfull, turn off the timer";
                chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
            }
            break;
            //Publishing
        case 1:
            LCMDBG_ << "[Publishing] Send another registration pack to mds";
            sendUnitServerRegistration();
            break;
            //Published
        case 2:
            LCMDBG_ << "[Published] Unit server registration completed, turn off the timer";
            chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
            
            LCMDBG_ << "[Published] Start control units registration state machine";
            startControlUnitSMThread();
            break;
            //Published failed
        case 3:
            LCMDBG_ << "[Published failed] Perform Unpublishing state";
            chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
            use_unit_server = false;
            break;
    }
}

//!prepare and send registration pack to the metadata server
void ControlManager::sendUnitServerRegistration() {
    chaos_data::CDataWrapper unit_server_registration_pack;
    //set server alias
    unit_server_registration_pack.addStringValue(NodeDefinitionKey::NODE_UNIQUE_ID, unit_server_alias);
    unit_server_registration_pack.addStringValue(NodeDefinitionKey::NODE_TYPE, NodeType::NODE_TYPE_UNIT_SERVER);
    
    if(unit_server_key.size()) {
        //the key need to be forwarded
        unit_server_registration_pack.addStringValue(NodeDefinitionKey::NODE_SECURITY_KEY, unit_server_key);
    }
    
    //add control unit alias
    for(MapCUAliasInstancerIterator iter = map_cu_alias_instancer.begin();
        iter != map_cu_alias_instancer.end();
        iter++) {
        unit_server_registration_pack.appendStringToArray(iter->first);
    }
    unit_server_registration_pack.finalizeArrayForKey(UnitServerNodeDefinitionKey::UNIT_SERVER_HOSTED_CONTROL_UNIT_CLASS);
    mds_channel->sendNodeRegistration(unit_server_registration_pack);
}

// Server registration ack message
CDataWrapper* ControlManager::unitServerRegistrationACK(CDataWrapper *message_data, bool &detach) throw (CException) {
    //lock the sm access
    boost::unique_lock<boost::shared_mutex> lock_sm(unit_server_sm_mutex);
    LCMAPP_ << "Unit server registration ack message received";
    detach = false;
    if(!message_data->hasKey(NodeDefinitionKey::NODE_UNIQUE_ID))
        throw CException(-1, "No alias forwarder", __PRETTY_FUNCTION__);
    
    string server_alias = message_data->getStringValue(NodeDefinitionKey::NODE_UNIQUE_ID);
    if(server_alias.compare(unit_server_alias) != 0) {
        throw CException(-2, "Server alias not found", __PRETTY_FUNCTION__);
    }
    if(message_data->hasKey(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT)) {
        //registration has been ended
        switch(message_data->getInt32Value(MetadataServerNodeDefinitionKeyRPC::PARAM_REGISTER_NODE_RESULT)){
            case ErrorCode::EC_MDS_NODE_REGISTRATION_OK:
                
                if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypePublished()) == boost::msm::back::HANDLED_TRUE){
                    LCMAPP_ << "Registration is gone well";
                    //we are published and it is ok!
                    //update healt status
                    HealtManager::getInstance()->addNodeMetricValue(unit_server_alias,
                                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_LOAD);
                } else {
                    LCMAPP_ << "Registration ACK received,bad  SM state "<<(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypePublished()));
                    throw CException(ErrorCode::EC_MDS_NODE_BAD_SM_STATE, "Bad state of the sm for published event", __PRETTY_FUNCTION__);
                }
                break;
                
            case ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_DUPLICATE_ALIAS:
                LCMERR_ << "The " << unit_server_alias << " is already used";
                //turn of unit server
                if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypeFailure()) == boost::msm::back::HANDLED_TRUE){
                    //we have problem
                    //update healt status
                    HealtManager::getInstance()->addNodeMetricValue(unit_server_alias,
                                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_UNLOAD);
                } else {
                    throw CException(ErrorCode::EC_MDS_NODE_BAD_SM_STATE, "Bad state of the sm for unpublishing event", __PRETTY_FUNCTION__);
                }
                break;
                
            case ErrorCode::EC_MDS_NODE_REGISTRATION_FAILURE_INVALID_ALIAS:
                LCMERR_ << "The " << unit_server_alias << " is invalid";
                //turn of unit server
                LCMDBG_ << "Turning of unit server";
                if(unit_server_sm.process_event(unit_server_state_machine::UnitServerEventType::UnitServerEventTypeFailure()) == boost::msm::back::HANDLED_TRUE){
                    //we have problem
                    HealtManager::getInstance()->addNodeMetricValue(unit_server_alias,
                                                                    NodeHealtDefinitionKey::NODE_HEALT_STATUS,
                                                                    NodeHealtDefinitionValue::NODE_HEALT_STATUS_UNLOAD);
                } else {
                    throw CException(ErrorCode::EC_MDS_NODE_BAD_SM_STATE, "Bad state of the sm for unpublished event", __PRETTY_FUNCTION__);
                }
                break;
        }
        
        //publish the healt result
        HealtManager::getInstance()->publishNodeHealt(unit_server_alias);
        
        //repeat fast as possible the timer
        chaos_async::AsyncCentralManager::getInstance()->removeTimer(this);
        chaos_async::AsyncCentralManager::getInstance()->addTimer(this, 0, GlobalConfiguration::getInstance()->getOption<uint64_t>(CONTROL_MANAGER_UNIT_SERVER_REGISTRATION_RETRY_MSEC));
    } else {
        throw CException(-3, "No result received", __PRETTY_FUNCTION__);
    }
    return NULL;
}

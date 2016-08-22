/*
 *	PublishingManager.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 19/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/publishing/PublishingManager.h>

using namespace chaos::common::utility;
using namespace chaos::cu::data_manager::publishing;

#define INFO    INFO_LOG(PublishingManager)
#define DBG     DBG_LOG(PublishingManager)
#define ERR     ERR_LOG(PublishingManager)

#pragma mark PublishableTargetManagerElement
PublishableTargetManagerElement::PublishableTargetManagerElement():
target(),
auto_remove(false){}

PublishableTargetManagerElement::PublishableTargetManagerElement(const PublishableTargetManagerElement& src):
target(src.target),
auto_remove(src.auto_remove){}

PublishableTargetManagerElement::PublishableTargetManagerElement(boost::shared_ptr<PublishTarget> _target,
                                                                 bool _auto_remove):
target(_target),
auto_remove(_auto_remove){}

#pragma mark PublishingManager Public Method

PublishingManager::PublishingManager(LockableObject<DatasetElementContainer>& _container_dataset):
container_dataset(_container_dataset){}

PublishingManager::~PublishingManager(){}

bool PublishingManager::addNewTarget(const std::string& target_name,
                                     bool auto_remove){
    LockableObjectWriteLock wl;
    map_name_target.getWriteLock(wl);
    //we can add the new target
    if(map_name_target().count(target_name) != 0) return false;
    
    PublishableTargetManagerElementShrdPtr new_element(new PublishableTargetManagerElement( boost::shared_ptr<PublishTarget>(new PublishTarget(target_name)),
                                                                                           auto_remove));
    map_name_target().insert(PublishableElementNameMapPair(target_name, new_element));
    
    INFO << CHAOS_FORMAT("Added new target %1% with auto-remove flag %2%", %target_name%auto_remove);
    return true;
}

bool PublishingManager::removeTarget(const std::string& target_name){
    LockableObjectWriteLock wl;
    map_name_target.getWriteLock(wl);
    //we can add the new target
    if(map_name_target().count(target_name) == 0) return false;
    
    map_name_target().erase(target_name);
    
    INFO << CHAOS_FORMAT("Removed new target %1%", %target_name);
    return true;
}

bool PublishingManager::addURLToTarget(const std::string& target_name,
                                       const std::string& server_url_new) {
    LockableObjectReadLock rl_target;
    map_name_target.getReadLock(rl_target);
    INFO << CHAOS_FORMAT("Add new URL[%1%] to target %2%", %server_url_new%target_name);

    //search target
    PublishableElementNameMapIterator target_found = map_name_target().find(target_name);
    if(target_found == map_name_target().end()){
        ERR << CHAOS_FORMAT("No target with name %1% found", %target_name);
        return false;
    }
    target_found->second->target->addServer(target_name);
    return true;
}

bool PublishingManager::removeURLServer(const std::string& target_name,
                                        const std::string& server_url_erase) {
    LockableObjectReadLock rl_target;
    map_name_target.getReadLock(rl_target);
    INFO << CHAOS_FORMAT("Remove URL[%1%] to target %2%", %server_url_erase%target_name);

    //search target
    PublishableElementNameMapIterator target_found = map_name_target().find(target_name);
    if(target_found == map_name_target().end()){
        ERR << CHAOS_FORMAT("No target with name %1% found", %target_name);
        return false;
    }
    target_found->second->target->removeServer(target_name);
    return true;
}


//!add a dataset into a target
bool PublishingManager::addDatasetToTarget(const std::string& target_name,
                                           const std::string& dataset_name,
                                           const PublishElementAttribute& publishable_attribute){
    LockableObjectReadLock rl_dataset;
    LockableObjectReadLock rl_target;
    
    map_name_target.getReadLock(rl_target);
    //search target
    PublishableElementNameMapIterator target_found = map_name_target().find(target_name);
    if(target_found == map_name_target().end()){
        DBG << CHAOS_FORMAT("No target with name %1% found", %target_name);
        return false;
    };
    
    //search dataset
    container_dataset.getReadLock(rl_dataset);
    DECNameIndex&   ds_index_name = container_dataset().get<DatasetElementTagName>();
    DECNameIndexIterator ds_found = ds_index_name.find(dataset_name);
    if(ds_found == ds_index_name.end()) {
        DBG << CHAOS_FORMAT("No dataset with name %1% found", %dataset_name);
        return false;
    }
    
    //we can associate dataset to the target
    target_found->second->target->addDataset(*(*ds_found), publishable_attribute);
    
    INFO << CHAOS_FORMAT("Associated dataset %1% to target %2%", %dataset_name%target_name);
    return true;
}

bool PublishingManager::removeDatasetFromTarget(const std::string& target_name,
                                                const std::string& dataset_name){
    LockableObjectReadLock rl_dataset;
    LockableObjectReadLock rl_target;
    
    map_name_target.getReadLock(rl_target);
    //search target
    PublishableElementNameMapIterator target_found = map_name_target().find(target_name);
    if(target_found == map_name_target().end()){
        DBG << CHAOS_FORMAT("No target with name %1% found", %target_name);
        return false;
    };
    
    //search dataset
    container_dataset.getReadLock(rl_dataset);
    DECNameIndex&   ds_index_name = container_dataset().get<DatasetElementTagName>();
    DECNameIndexIterator ds_found = ds_index_name.find(dataset_name);
    if(ds_found == ds_index_name.end()) {
        DBG << CHAOS_FORMAT("No dataset with name %1% found", %dataset_name);
        return false;
    }
    //we can associate dataset to the target
    target_found->second->target->removeDataset((*ds_found)->dataset->getDatasetName());
    
    INFO << CHAOS_FORMAT("Removed dataset %1% from target %2%", %dataset_name%target_name);
    return true;
}

void PublishingManager::setPublishAttributeOnDataset(const std::string& target_name,
                                                     const std::string& dataset_name,
                                                     const PublishElementAttribute& publishable_attribute) {
    
    LockableObjectReadLock rl_target;
    map_name_target.getReadLock(rl_target);
    //search target
    PublishableElementNameMapIterator target_found = map_name_target().find(target_name);
    if(target_found == map_name_target().end()){
        DBG << CHAOS_FORMAT("No target with name %1% found", %target_name);
    }
    target_found->second->target->setAttributeOnDataset(target_name,
                                                        publishable_attribute);
    DBG << CHAOS_FORMAT("Publishing attribute changed for dataset %1% in target %2%", %dataset_name%target_name);
}

#pragma mark PublishingManager Private Method

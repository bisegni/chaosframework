/*
 *	StatusFlagCatalog.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 12/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/status_manager/StatusFlagCatalog.h>

using namespace chaos::common::data;
using namespace chaos::common::status_manager;

StatusFlagCatalog::StatusFlagCatalog(const std::string& _catalog_name):
catalog_name(_catalog_name){}

StatusFlagCatalog::~StatusFlagCatalog(){}

void StatusFlagCatalog::addFlag(boost::shared_ptr<StatusFlag> flag) {
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    StatusFlagElementContainerNameIndex& name_index = catalog_container.get<name>();
    if(name_index.find(flag->name) != name_index.end()) return;
    //we can insert flag with unique name
    catalog_container.insert(StatusFlagElement::StatusFlagElementPtr(new StatusFlagElement((unsigned int)(catalog_container.size()+1), flag->name, flag)));
}

void StatusFlagCatalog::appendCatalog(const StatusFlagCatalog& src) {
    //write loc on loca catalog
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    
    //read lock on source catalog
    boost::shared_lock<boost::shared_mutex> rl(src.mutex_catalog);
    //retrieve the ordered index
    StatusFlagElementContainerNameIndex& name_index = catalog_container.get<name>();
    const StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<ordered>(src.catalog_container);
    for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        const std::string cat_flag_name = src.catalog_name + (*it)->flag_name;
        if(name_index.find(cat_flag_name) != name_index.end()) continue;
        //we can insert
        catalog_container.insert(StatusFlagElement::StatusFlagElementPtr(new StatusFlagElement((unsigned int)(catalog_container.size()+1), cat_flag_name, (*it)->status_flag)));
    }
}

StatusFlag *StatusFlagCatalog::getFlagByName(const std::string& flag_name) {
    return NULL;
}

bool StatusFlagCatalog::hasChanged() {
    return bitmap_changed_flag.any();
}

void StatusFlagCatalog::resetChanged() {
    bitmap_changed_flag.reset();
}

#pragma mark Serialization Method
std::auto_ptr<chaos::common::data::CDataBuffer> StatusFlagCatalog::getRawFlagsLevel() {
    //read lock on owned catalog
    boost::shared_lock<boost::shared_mutex> rl(mutex_catalog);
    std::auto_ptr<CDataBuffer> result;
    char * raw_description = (char*)malloc(catalog_container.size());
    if(raw_description) {
        //retrieve the ordered index
        StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<ordered>(catalog_container);
        for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){
            raw_description[(*it)->seq_id] = static_cast<char>((*it)->status_flag->getCurrentLevel());
        }
        result.reset(CDataBuffer::newOwnBufferFromBuffer(raw_description,
                                                         (uint32_t)catalog_container.size()));
    }
    return result;
}

void StatusFlagCatalog::setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer> raw_level) {
    if(raw_level.get() == NULL) return;
    const char * buffer = raw_level->getBuffer();
    uint32_t buffer_size = raw_level->getBufferSize();
    
    if(buffer_size != catalog_container.size()) return;
    
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    for(int idx = 0;
        idx < raw_level->getBufferSize();
        idx++) {
        StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<ordered>(catalog_container);
        for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){
            
            (*it)->status_flag->setCurrentLevel(buffer[(*it)->seq_id]);
        }
    }
}


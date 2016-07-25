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

#define SLC_LOG INFO_LOG_1_P(StatusFlag, catalog_name)
#define SLC_DBG DBG_LOG_1_P(StatusFlag, catalog_name)
#define SLC_ERR ERR_LOG_1_p(StatusFlag, catalog_name)

StatusFlagCatalog::StatusFlagCatalog():
catalog_name(){
    //iitializethe severity map
    for(int s = StatusFlagServerityRegular;
        s != StatusFlagServerityUndefuned;
        ++s) {
        map_severity_bf_flag.insert(MapSeverityBitfieldPair((StatusFlagServerity)s, boost::dynamic_bitset<uint8_t>()));
    }
}

StatusFlagCatalog::StatusFlagCatalog(const std::string& _catalog_name):
catalog_name(_catalog_name){
    //iitializethe severity map
    for(int s = StatusFlagServerityRegular;
        s != StatusFlagServerityUndefuned;
        ++s) {
        map_severity_bf_flag.insert(MapSeverityBitfieldPair((StatusFlagServerity)s, boost::dynamic_bitset<uint8_t>()));
    }
}

StatusFlagCatalog::~StatusFlagCatalog(){
    //!remove listere to all flag
    StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container);
    for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        (*it)->status_flag->removeListener(this);
    }
}


void StatusFlagCatalog::statusFlagUpdated(const std::string& flag_uuid,
                                          const StatusFlagServerity current_level_severity) {
    StatusFlagElementContainerFlaUUIDIndex& uuid_index = catalog_container.get<mitag_flag_uuid>();
    StatusFlagElementContainerFlaUUIDIndexItarator src_flag_it = uuid_index.find(flag_uuid);
    if(src_flag_it == uuid_index.end()) return;
    SLC_DBG << "Signal from " << (*src_flag_it)->flag_name << "with severity" << current_level_severity;
    
    for(int s = StatusFlagServerityRegular;
        s != StatusFlagServerityUndefuned;
        ++s) {
        if(s == current_level_severity) {
            map_severity_bf_flag[(StatusFlagServerity)s].set((*src_flag_it)->seq_id);
        } else {
            map_severity_bf_flag[(StatusFlagServerity)s].reset((*src_flag_it)->seq_id);
        }
        DEBUG_CODE(
                   std::string buffer;
                   boost::to_string(map_severity_bf_flag[(StatusFlagServerity)s], buffer);
                   SLC_DBG << "Bitfiled for severity "<< (StatusFlagServerity)s <<"representation:" << buffer;
                   );
    }
}

void StatusFlagCatalog::addMemberToSeverityMap(boost::shared_ptr<StatusFlag> new_status_flag) {
    DEBUG_CODE(SLC_DBG << "Add fag to severity bitfield map " << new_status_flag->getName(););
    //iitializethe severity map
    for(int s = StatusFlagServerityRegular;
        s != StatusFlagServerityUndefuned;
        ++s) {
        if(s == new_status_flag->getCurrentStateLevel().getSeverity()) {
            map_severity_bf_flag[(StatusFlagServerity)s].push_back(1);
        } else {
            map_severity_bf_flag[(StatusFlagServerity)s].push_back(0);
        }
        DEBUG_CODE(
                   std::string buffer;
                   boost::to_string(map_severity_bf_flag[(StatusFlagServerity)s], buffer);
                   SLC_DBG << "Bitfiled for severity "<< (StatusFlagServerity)s <<"representation:" << buffer;
                   );
    }
    
}

void StatusFlagCatalog::addFlag(boost::shared_ptr<StatusFlag> flag) {
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    StatusFlagElementContainerNameIndex& name_index = catalog_container.get<mitag_name>();
    if(name_index.find(flag->getName()) != name_index.end()) return;
    
    //we can insert flag with unique name
    catalog_container.insert(StatusFlagElement::StatusFlagElementPtr(new StatusFlagElement((unsigned int)catalog_container.size(), flag->getName(), flag)));
    
    addMemberToSeverityMap(flag);
    
    //add this catalog as listener
    flag->addListener(this);
}

void StatusFlagCatalog::appendCatalog(const StatusFlagCatalog& src) {
    //write loc on loca catalog
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    
    //read lock on source catalog
    boost::shared_lock<boost::shared_mutex> rl(src.mutex_catalog);
    //retrieve the ordered index
    StatusFlagElementContainerNameIndex& name_index = catalog_container.get<mitag_name>();
    const StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(src.catalog_container);
    for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        const std::string cat_flag_name = src.catalog_name + "/" + (*it)->flag_name;
        if(name_index.find(cat_flag_name) != name_index.end()) continue;
        //we can insert
        catalog_container.insert(StatusFlagElement::StatusFlagElementPtr(new StatusFlagElement((unsigned int)catalog_container.size(), cat_flag_name, (*it)->status_flag)));
        
        addMemberToSeverityMap((*it)->status_flag);
        
        (*it)->status_flag->addListener(this);
    }
}

boost::shared_ptr<StatusFlag> StatusFlagCatalog::getFlagByName(const std::string& flag_name) {
    StatusFlagElementContainerNameIndex& name_index = catalog_container.get<mitag_name>();
    StatusFlagElementContainerNameIterator nit = name_index.find(flag_name);
    if(nit == name_index.end()) return boost::shared_ptr<StatusFlag>();
    return (*nit)->status_flag;
}

boost::shared_ptr<StatusFlag> StatusFlagCatalog::getFlagByOrderedID(const unsigned int flag_ordered_id) {
    StatusFlagElementContainerOrderedIndex& ordered_index = catalog_container.get<mitag_ordered>();
    StatusFlagElementContainerOrderedIndexIterator nit = ordered_index.find(flag_ordered_id);
    if(nit == ordered_index.end()) return boost::shared_ptr<StatusFlag>();
    return (*nit)->status_flag;
}

void StatusFlagCatalog::getFlagsForSeverity(StatusFlagServerity severity,
                                            VectorStatusFlag& found_flag) {
    //iterate on active flag for severity
    size_t index = map_severity_bf_flag[severity].find_first();
    while(index != boost::dynamic_bitset<uint8_t>::npos) {
        found_flag.push_back(getFlagByOrderedID((unsigned int)index));
        
        //nex index
        index = map_severity_bf_flag[severity].find_next(index);
    }
}

#pragma mark Serialization Method
std::auto_ptr<chaos::common::data::CDataBuffer> StatusFlagCatalog::getRawFlagsLevel() {
    //read lock on owned catalog
    boost::shared_lock<boost::shared_mutex> rl(mutex_catalog);
    std::auto_ptr<CDataBuffer> result;
    char * raw_description = (char*)malloc(catalog_container.size());
    if(raw_description) {
        //retrieve the ordered index
        StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container);
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
        StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container);
        for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){
            
            (*it)->status_flag->setCurrentLevel(buffer[(*it)->seq_id]);
        }
    }
}

const size_t StatusFlagCatalog::size() const {
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    return catalog_container.size();
}

const std::string& StatusFlagCatalog::getName() const {
    return catalog_name;
}

StatusFlagCatalog& StatusFlagCatalog::operator=(StatusFlagCatalog const &rhs) {
    catalog_name = rhs.catalog_name;
    catalog_container = rhs.catalog_container;
    //gester this new container has listener
    StatusFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container);
    for(StatusFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        
        (*it)->status_flag->addListener(this);
    }
    return *this;
}

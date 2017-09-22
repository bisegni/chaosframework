/*
 * Copyright 2012, 2017 INFN
 *
 * Licensed under the EUPL, Version 1.2 or – as soon they
 * will be approved by the European Commission - subsequent
 * versions of the EUPL (the "Licence");
 * You may not use this work except in compliance with the
 * Licence.
 * You may obtain a copy of the Licence at:
 *
 * https://joinup.ec.europa.eu/software/page/eupl
 *
 * Unless required by applicable law or agreed to in
 * writing, software distributed under the Licence is
 * distributed on an "AS IS" basis,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied.
 * See the Licence for the specific language governing
 * permissions and limitations under the Licence.
 */

#include <chaos/common/global.h>
#include <chaos/common/state_flag/StateFlagCatalog.h>

using namespace chaos::common::data;
using namespace chaos::common::utility;
using namespace chaos::common::state_flag;

#define SLC_LOG INFO_LOG_1_P(StateFlag, catalog_name)
#define SLC_DBG DBG_LOG_1_P(StateFlag, catalog_name)
#define SLC_ERR ERR_LOG_1_p(StateFlag, catalog_name)

StateFlagCatalog::StateFlagCatalog():
catalog_name(){
    //iitializethe severity map
    for(int s = StateFlagServerityRegular;
        s != StateFlagServerityUndefuned;
        ++s) {
        map_severity_bf_flag.insert(MapSeverityBitfieldPair((StateFlagServerity)s, boost::dynamic_bitset<uint8_t>()));
    }
}

StateFlagCatalog::StateFlagCatalog(const std::string& _catalog_name):
catalog_name(_catalog_name){
    //iitializethe severity map
    for(int s = StateFlagServerityRegular;
        s != StateFlagServerityUndefuned;
        ++s) {
        map_severity_bf_flag.insert(MapSeverityBitfieldPair((StateFlagServerity)s, boost::dynamic_bitset<uint8_t>()));
    }
}

StateFlagCatalog::StateFlagCatalog(const StateFlagCatalog& _catalog) {
    *this = _catalog;
}

StateFlagCatalog::~StateFlagCatalog(){
    //!remove listere to all flag
    StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container());
    for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        (*it)->status_flag->removeListener(this);
    }
}


void StateFlagCatalog::stateFlagUpdated(const FlagDescription       flag_description,
                                        const std::string&          level_name,
                                        const StateFlagServerity    current_level_severity) {
    StateFlagElementContainerFlaUUIDIndex& uuid_index = catalog_container().get<mitag_flag_uuid>();
    StateFlagElementContainerFlaUUIDIndexItarator src_flag_it = uuid_index.find(flag_description.uuid);
    if(src_flag_it == uuid_index.end()) return;
    SLC_DBG << "Signal from " << (*src_flag_it)->flag_name << "with severity" << current_level_severity;
    
    for(int s = StateFlagServerityRegular;
        s != StateFlagServerityUndefuned;
        ++s) {
        if(s == current_level_severity) {
            map_severity_bf_flag[(StateFlagServerity)s].set((*src_flag_it)->seq_id);
        } else {
            map_severity_bf_flag[(StateFlagServerity)s].reset((*src_flag_it)->seq_id);
        }
        DEBUG_CODE(
                   std::string buffer;
                   boost::to_string(map_severity_bf_flag[(StateFlagServerity)s], buffer);
                   SLC_DBG << "Bitfiled for severity "<< (StateFlagServerity)s <<"representation:" << buffer;
                   );
    }
}

void StateFlagCatalog::addMemberToSeverityMap(ChaosSharedPtr<StateFlag> new_status_flag) {
    DEBUG_CODE(SLC_DBG << "Add fag to severity bitfield map " << new_status_flag->getName(););
    //iitializethe severity map
    for(int s = StateFlagServerityRegular;
        s != StateFlagServerityUndefuned;
        ++s) {
        if(s == new_status_flag->getCurrentStateLevel().getSeverity()) {
            map_severity_bf_flag[(StateFlagServerity)s].push_back(1);
        } else {
            map_severity_bf_flag[(StateFlagServerity)s].push_back(0);
        }
        DEBUG_CODE(std::string buffer;
                   boost::to_string(map_severity_bf_flag[(StateFlagServerity)s], buffer);
                   SLC_DBG << "Bitfiled for severity "<< (StateFlagServerity)s <<"representation:" << buffer;);
    }
    
}

void StateFlagCatalog::addFlag(const ChaosSharedPtr<StateFlag>& flag) {
    //boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    LockableObjectWriteLock_t wl;
    catalog_container.getWriteLock(wl);
    StateFlagElementContainerNameIndex& name_index = catalog_container().get<mitag_name>();
    if(name_index.find(flag->getName()) != name_index.end()) return;
    //we can insert flag with unique name
    catalog_container().insert(StateFlagElement::StateFlagElementPtr(new StateFlagElement((unsigned int)catalog_container().size(), flag->getName(), flag)));
    
    addMemberToSeverityMap(flag);
    
    //add this catalog as listener
    flag->addListener(this);
}

void StateFlagCatalog::setFlagState(const std::string& flag_name, int8_t new_state) {
    StateFlagElementContainerNameIndex& name_index = catalog_container().get<mitag_name>();
    StateFlagElementContainerNameIterator nit = name_index.find(flag_name);
    if(nit == name_index.end()) return;
    (*nit)->status_flag->setCurrentLevel(new_state);
}

void StateFlagCatalog::setFlagState(const unsigned int flag_ordered_id, int8_t new_state) {
    StateFlagElementContainerOrderedIndex& ordered_index = catalog_container().get<mitag_ordered>();
    StateFlagElementContainerOrderedIndexIterator nit = ordered_index.find(flag_ordered_id);
    if(nit == ordered_index.end()) return;
    (*nit)->status_flag->setCurrentLevel(new_state);
}

void StateFlagCatalog::setAllFlagState(int8_t new_state) {
    StateFlagElementContainerOrderedIndex& ordered_index = catalog_container().get<mitag_ordered>();
    //set the state into all flag
    for(StateFlagElementContainerOrderedIndexIterator nit = ordered_index.begin(),
        nit_end = ordered_index.end();
        nit != nit_end;
        nit++) {
        (*nit)->status_flag->setCurrentLevel(new_state);
    }
}

void StateFlagCatalog::appendCatalog(const StateFlagCatalog& src) {
    //write loc on loca catalog
    //boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    LockableObjectWriteLock_t wl;
    LockableObjectReadLock_t rl_src;
    catalog_container.getWriteLock(wl);
    
    //read lock on source catalog
    src.catalog_container.getReadLock(rl_src);
    //boost::shared_lock<boost::shared_mutex> rl();
    //retrieve the ordered index
    StateFlagElementContainerNameIndex& name_index = catalog_container().get<mitag_name>();
    const StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(src.catalog_container());
    for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        const std::string cat_flag_name = src.catalog_name + "/" + (*it)->flag_name;
        if(name_index.find(cat_flag_name) != name_index.end()) continue;
        //we can insert
        catalog_container().insert(StateFlagElement::StateFlagElementPtr(new StateFlagElement((unsigned int)catalog_container().size(), cat_flag_name, (*it)->status_flag)));
        
        addMemberToSeverityMap((*it)->status_flag);
        
        (*it)->status_flag->addListener(this);
    }
}

ChaosSharedPtr<StateFlag>& StateFlagCatalog::getFlagByName(const std::string& flag_name) {
    StateFlagElementContainerNameIndex& name_index = catalog_container().get<mitag_name>();
    StateFlagElementContainerNameIterator nit = name_index.find(flag_name);
    if(nit == name_index.end()) return empty_flag;
    return (*nit)->status_flag;
}

ChaosSharedPtr<StateFlag>& StateFlagCatalog::getFlagByOrderedID(const unsigned int flag_ordered_id) {
    StateFlagElementContainerOrderedIndex& ordered_index = catalog_container().get<mitag_ordered>();
    StateFlagElementContainerOrderedIndexIterator nit = ordered_index.find(flag_ordered_id);
    if(nit == ordered_index.end()) return empty_flag;
    return (*nit)->status_flag;
}

void StateFlagCatalog::getFlagsForSeverity(StateFlagServerity severity,
                                           VectorStateFlag& found_flag) {
    //iterate on active flag for severity
    size_t index = map_severity_bf_flag[severity].find_first();
    while(index != boost::dynamic_bitset<uint8_t>::npos) {
        found_flag.push_back(getFlagByOrderedID((unsigned int)index));
        
        //nex index
        index = map_severity_bf_flag[severity].find_next(index);
    }
}

#pragma mark Serialization Method
ChaosUniquePtr<chaos::common::data::CDataBuffer> StateFlagCatalog::getRawFlagsLevel() {
    //read lock on owned catalog
    LockableObjectReadLock_t rl;
    catalog_container.getReadLock(rl);
    ChaosUniquePtr<CDataBuffer> result;
    char * raw_description = (char*)malloc(catalog_container().size());
    if(raw_description) {
        //retrieve the ordered index
        StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container());
        for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){
            raw_description[(*it)->seq_id] = static_cast<char>((*it)->status_flag->getCurrentLevel());
        }
        result.reset(CDataBuffer::newOwnBufferFromBuffer(raw_description,
                                                         (uint32_t)catalog_container().size()));
    }
    return result;
}

void StateFlagCatalog::setApplyRawFlagsValue(ChaosUniquePtr<chaos::common::data::CDataBuffer>& raw_level) {
    if(raw_level.get() == NULL) return;
    const char * buffer = raw_level->getBuffer();
    uint32_t buffer_size = raw_level->getBufferSize();
    
    if(buffer_size != catalog_container().size()) return;
    LockableObjectWriteLock_t wl;
    catalog_container.getWriteLock(wl);
    //boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    for(int idx = 0;
        idx < raw_level->getBufferSize();
        idx++) {
        StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container());
        for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){
            
            (*it)->status_flag->setCurrentLevel(buffer[(*it)->seq_id]);
        }
    }
}

const size_t StateFlagCatalog::size() const {
    LockableObjectWriteLock_t wl;
    catalog_container.getWriteLock(wl);
    //boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    return catalog_container().size();
}

const uint8_t StateFlagCatalog::max() const {
	uint8_t maxv=0;
    LockableObjectWriteLock_t wl;
    catalog_container.getWriteLock(wl);
    //boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container());
        for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
            end = ordered_index.end();
            it != end;
            it++){

            maxv=(maxv<(*it)->status_flag->getCurrentLevel())?(*it)->status_flag->getCurrentLevel():maxv;
        }
    return maxv;
}
const std::string& StateFlagCatalog::getName() const {
    return catalog_name;
}

StateFlagCatalog& StateFlagCatalog::operator=(StateFlagCatalog const &rhs) {
    catalog_name = rhs.catalog_name;
    catalog_container() = rhs.catalog_container();
    //gester this new container has listener
    StateFlagElementContainerOrderedIndex& ordered_index = boost::multi_index::get<mitag_ordered>(catalog_container());
    for(StateFlagElementContainerOrderedIndexIterator it = ordered_index.begin(),
        end = ordered_index.end();
        it != end;
        it++){
        
        (*it)->status_flag->addListener(this);
    }
    return *this;
}

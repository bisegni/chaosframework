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

void StatusFlagCatalog::addFlag(const StatusFlag& flag) {
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    catalog.insert(MapSFCatalogPair((unsigned int)(catalog.size()+1), flag));
}

void StatusFlagCatalog::appendCatalog(const StatusFlagCatalog& src) {
    //write loc on loca catalog
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    
    //read lock on source catalog
    boost::shared_lock<boost::shared_mutex> rl(src.mutex_catalog);
    for(MapSFCatalogConstIterator it = src.catalog.begin(),
        end = src.catalog.end();
        it != end;
        it++){
        catalog.insert(MapSFCatalogPair((unsigned int)(catalog.size()+1), it->second));
    }
}

std::auto_ptr<chaos::common::data::CDataBuffer> StatusFlagCatalog::getRawFlagsLevel() {
    //read lock on owned catalog
    boost::shared_lock<boost::shared_mutex> rl(mutex_catalog);
    std::auto_ptr<CDataBuffer> result;
    char * raw_description = (char*)malloc(catalog.size());
    if(raw_description) {
        for(MapSFCatalogConstIterator it = catalog.begin(),
            end = catalog.end();
            it != end;
            it++){
            raw_description[it->first] = static_cast<char>(it->second.getCurrentLevel());
        }
        result.reset(CDataBuffer::newOwnBufferFromBuffer(raw_description,
                                                         (uint32_t)catalog.size()));
    }
    return result;
}

void StatusFlagCatalog::setApplyRawFlagsValue(std::auto_ptr<chaos::common::data::CDataBuffer> raw_level) {
    if(raw_level.get() == NULL) return;
    const char * buffer = raw_level->getBuffer();
    uint32_t buffer_size = raw_level->getBufferSize();
    
    boost::unique_lock<boost::shared_mutex> wl(mutex_catalog);
    for(int idx = 0;
        idx < raw_level->getBufferSize();
        idx++) {
        for(MapSFCatalogIterator it = catalog.begin(),
            end = catalog.end();
            it != end;
            it++){
            if(it->first >= buffer_size) continue;
            //assign value
            it->second.setCurrentLevel(buffer[it->first]);
        }
    }
}

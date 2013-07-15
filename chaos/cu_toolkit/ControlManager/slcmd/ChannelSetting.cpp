/*
 *	ChannelSetting.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#include <string>
#include <chaos/cu_toolkit/ControlManager/slcmd/ChannelSetting.h>

using namespace std;
using namespace chaos::cu::control_manager::slow_command;

ValueSetting::ValueSetting(uint32_t _size, uint32_t _index):currentValue(NULL), nextValue(NULL), size(_size),index(_index) {
    currentValue = malloc(size);
    nextValue = malloc(size);
}

ValueSetting::~ValueSetting() {
    if(currentValue) free(currentValue);
    if(nextValue) free(nextValue);
}

void ValueSetting::completed() {
    std::memcpy(currentValue, nextValue, size);
    std::memset(nextValue, 0, size);
    (*sharedBitmapChangedAttribute)[index]=0;
}

bool ValueSetting::setDestinationValue(void* valPtr, uint32_t _size) {
    if(_size>size) return false;
    
    //copy the new value
    std::memcpy(currentValue, nextValue, _size);
    
    //set the relative field for set has changed
    (*sharedBitmapChangedAttribute)[index]=1;
    return true;
}

//-----------------------------------------------------------------------------------------------------------------------------

ChannelSetting::ChannelSetting() {
    bitmapChangedAttribute = NULL;
}

ChannelSetting::~ChannelSetting() {
    if(bitmapChangedAttribute) delete(bitmapChangedAttribute);
}

//! Initialize instance
void ChannelSetting::init(void *initData) throw(chaos::CException) {

    
    bitmapChangedAttribute = new boost::dynamic_bitset<BIT_BLOCK_DIMENSION>(mapAttributeIndexSettings.size());
    if(!bitmapChangedAttribute) throw CException(1, "Error allocating memory for map bit", "ChannelSetting::init");
   
    for (map<ATTRIBUTE_INDEX_TYPE, ValueSetting*>::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        it->second->sharedBitmapChangedAttribute = bitmapChangedAttribute;
    }
    
    bitmapChangedAttribute->clear();
}

//! Deinit the implementation
void ChannelSetting::deinit() throw(chaos::CException) {
    //remove all ValueSetting instance
    for (map<ATTRIBUTE_INDEX_TYPE, ValueSetting*>::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        delete(it->second);
    }
    
    if(bitmapChangedAttribute) {
       delete(bitmapChangedAttribute);
        bitmapChangedAttribute = NULL;
    }
   
}

//!
void ChannelSetting::addAttribute(string name, uint32_t size) {
    
    if(InizializableService::serviceState != utility::InizializableServiceType::IS_DEINTIATED)
        throw CException(1, "Attribute can be added only in deinitilized state", "ChannelSetting::addAttribute");
    
    if(mapAttributeNameIndex.count(name)) return;
    
    ATTRIBUTE_INDEX_TYPE tmpIndex;
    
    //add name nad his index
    mapAttributeNameIndex.insert(make_pair<string, ATTRIBUTE_INDEX_TYPE>(name, (tmpIndex=index++)));
    
    //add channel setting
    mapAttributeIndexSettings.insert(make_pair<ATTRIBUTE_INDEX_TYPE, ValueSetting*>(tmpIndex, new ValueSetting(size, tmpIndex)));
    
    //add the relative bit

}

//! set the value for the index
void ChannelSetting::setValueForAttribute(ATTRIBUTE_INDEX_TYPE n, void * value, uint32_t size) {
    mapAttributeIndexSettings[n]->setDestinationValue(value, size);
}

//!
void ChannelSetting::getChangedIndex(std::vector<ATTRIBUTE_INDEX_TYPE>& changedIndex) {
    size_t index = 0;
    changedIndex.push_back(index = bitmapChangedAttribute->find_first());
    while(index != boost::dynamic_bitset<BIT_BLOCK_DIMENSION>::npos)
    {
        /* do something */
        index = bitmapChangedAttribute->find_next(index);
    }
}

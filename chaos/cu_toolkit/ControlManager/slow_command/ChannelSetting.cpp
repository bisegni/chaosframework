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
#include <chaos/cu_toolkit/ControlManager/slow_command/ChannelSetting.h>

#define CSLAPP_ LAPP_ << "[ChannelSetting-" << "] "
#define CSLDBG_ LDBG_ << "[ChannelSetting-" << "] "
#define CSLERR_ LERR_ << "[ChannelSetting-" << "] "

using namespace std;
using namespace chaos::cu::control_manager::slow_command;

ValueSetting::ValueSetting(uint32_t _size, uint32_t _index, chaos::DataType::DataType _type):currentValue(NULL), nextValue(NULL), size(_size),index(_index), type(_type) {
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

bool ValueSetting::setDestinationValue(const void* valPtr, uint32_t _size) {
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
    bitmapChangedAttribute = new boost::dynamic_bitset<BitBlockDimension>(mapAttributeIndexSettings.size());
    if(!bitmapChangedAttribute) throw CException(1, "Error allocating memory for map bit", "ChannelSetting::init");
   
    for (map<AttributeIndexType, boost::shared_ptr<ValueSetting> >::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        it->second->sharedBitmapChangedAttribute = bitmapChangedAttribute;
    }
    
    bitmapChangedAttribute->clear();
}

//! Deinit the implementation
void ChannelSetting::deinit() throw(chaos::CException) {
    //remove all ValueSetting instance
   /* for (map<AttributeIndexType, boost::shared_ptr<ValueSetting> >::iterator it = mapAttributeIndexSettings.begin() ;
         it != mapAttributeIndexSettings.end();
         it++) {
        delete(it->second);
    }*/
    mapAttributeIndexSettings.clear();
    if(bitmapChangedAttribute) {
       delete(bitmapChangedAttribute);
        bitmapChangedAttribute = NULL;
    }
   
}

//!
void ChannelSetting::addAttribute(string name, uint32_t size, chaos::DataType::DataType type) {
    
    if(InizializableService::serviceState != utility::InizializableServiceType::IS_DEINTIATED)
        throw CException(1, "Attribute can be added only in deinitilized state", "ChannelSetting::addAttribute");
    
    if(mapAttributeNameIndex.count(name)) return;
    
    AttributeIndexType tmpIndex;
    
    //add name nad his index
    mapAttributeNameIndex.insert(make_pair<string, AttributeIndexType>(name, (tmpIndex=index++)));
    
    //add channel setting
    boost::shared_ptr<ValueSetting> tmpSP(new ValueSetting(size, tmpIndex, type));
    mapAttributeIndexSettings.insert(make_pair<AttributeIndexType, boost::shared_ptr<ValueSetting> >(tmpIndex, tmpSP));
    
    //add the relative bit

}

//! set the value for the index
void ChannelSetting::setValueForAttribute(AttributeIndexType n, const void * value, uint32_t size) {
    mapAttributeIndexSettings[n]->setDestinationValue(value, size);
}

AttributeIndexType ChannelSetting::getIndexForName( string name ) {
    return mapAttributeNameIndex[name];
}

ValueSetting *ChannelSetting::getValueSettingForIndex(AttributeIndexType index) {
    if(!mapAttributeIndexSettings.count(index)) return NULL;
        
    return mapAttributeIndexSettings[index].get();
}

//!
void ChannelSetting::getChangedIndex(std::vector<AttributeIndexType>& changedIndex) {
    size_t index = 0;
    changedIndex.push_back(index = bitmapChangedAttribute->find_first());
    while(index != boost::dynamic_bitset<BitBlockDimension>::npos)
    {
        /* do something */
        index = bitmapChangedAttribute->find_next(index);
    }
}


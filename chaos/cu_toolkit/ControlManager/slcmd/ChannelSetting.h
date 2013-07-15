/*
 *	ChannelSetting.h
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

#ifndef __CHAOSFramework__ChannelSetting__
#define __CHAOSFramework__ChannelSetting__

#include <map>
#include <string>

#include <boost/thread.hpp>
#include <boost/dynamic_bitset.hpp>

#include <chaos/common/utility/InizializableService.h>

#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandTypes.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace slow_command {
                
                using namespace std;
                
                class ChannelSetting;

                typedef void * SettingValuePtr;
                
            
                struct ValueSetting {
                    friend class ChannelSetting;
                    uint32_t index;
                    uint32_t size;
                    
                    SettingValuePtr currentValue;
                    SettingValuePtr nextValue;
                    boost::dynamic_bitset<BIT_BLOCK_DIMENSION> * sharedBitmapChangedAttribute;

                    ValueSetting(uint32_t _size, uint32_t _index);
                    
                    ~ValueSetting();
                    
                    void completed();
                    
                    bool setDestinationValue(void* valPtr, uint32_t _size);
                };

                //-----------------------------------------------------------------------------------------------------------------------------

                
                class ChannelSetting : public utility::InizializableService {
                    
                    ATTRIBUTE_INDEX_TYPE index;
                    boost::mutex mutextAccessMap;

                    boost::dynamic_bitset<BIT_BLOCK_DIMENSION> *bitmapChangedAttribute;
                    
                    map<string, ATTRIBUTE_INDEX_TYPE> mapAttributeNameIndex;
                    map<ATTRIBUTE_INDEX_TYPE, ValueSetting*> mapAttributeIndexSettings;
                public:
                    
                    ChannelSetting();
                    
                    ~ChannelSetting();
                    
                    void addAttribute(string name, uint32_t size);
                    
                    void setValueForAttribute(ATTRIBUTE_INDEX_TYPE n, void * value, uint32_t size);
                    
                    //! Initialize instance
                    void init(void *initData) throw(chaos::CException);
                    
                    //! Deinit the implementation
                    void deinit() throw(chaos::CException);
                    
                    void getChangedIndex(std::vector<ATTRIBUTE_INDEX_TYPE>& changedIndex);
                };
                
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__ChannelSetting__) */

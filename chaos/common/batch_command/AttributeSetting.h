/*
 *	AttributeSetting.h
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

#ifndef __CHAOSFramework__AttributeSetting__
#define __CHAOSFramework__AttributeSetting__

#include <map>
#include <string>

#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/dynamic_bitset.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/utility/InizializableService.h>

#include <chaos/common/batch_command/BatchCommandTypes.h>

namespace chaos{
    namespace common {
        namespace batch_command {
            
            using namespace std;
            
            class AttributeSetting;
            
            typedef void * SettingValuePtr;
            
            
            struct ValueSetting {
                friend class AttributeSetting;
                const uint32_t                      index;
                const uint32_t                      size;
                const chaos::DataType::DataType     type;
                boost::mutex                        mutextAccessSetting;
                void								*buffer;
                SettingValuePtr currentValue;
                SettingValuePtr nextValue;
                boost::dynamic_bitset<BitBlockDimension> * sharedBitmapChangedAttribute;
                
                ValueSetting(uint32_t _size, uint32_t _index, chaos::DataType::DataType type);
                
                ~ValueSetting();
                
                void completed();
                
                void completedWithError();
                
                bool setNextValue(const void* valPtr, uint32_t _size);
                
                bool setDefaultValue(const void* valPtr, uint32_t _size);
                
                template<typename T>
                T* getNextValue() {
                    return static_cast<T*>(nextValue);
                }
                
                template<typename T>
                T* getCurrentValue() {
                    return static_cast<T*>(currentValue);
                }
            };
            
            //-----------------------------------------------------------------------------------------------------------------------------
            
            
            class AttributeSetting : public chaos::utility::InizializableService {
                
                VariableIndexType index;
                
                boost::dynamic_bitset<BitBlockDimension> bitmapChangedAttribute;
                
                map<string, VariableIndexType> mapAttributeNameIndex;
                map<VariableIndexType, boost::shared_ptr<ValueSetting> > mapAttributeIndexSettings;
            public:
                
                AttributeSetting();
                
                ~AttributeSetting();
                
                void addAttribute(string name, uint32_t size, chaos::DataType::DataType type);
                
                void setValueForAttribute(VariableIndexType n, const void * value, uint32_t size);
                void setDefaultValueForAttribute(VariableIndexType n, const void * value, uint32_t size);
                
                VariableIndexType getIndexForName( string name );
                
                void getAttributeNames(std::vector<std::string>& names);
                
                //! Initialize instance
                void init(void *initData) throw(chaos::CException);
                
                //! Deinit the implementation
                void deinit() throw(chaos::CException);
                
                void getChangedIndex(std::vector<VariableIndexType>& changedIndex);
                
                ValueSetting *getValueSettingForIndex(VariableIndexType index);
            };
            
            //-----------------------------------------------------------------------------------------------------------------------------
            
            class SharedCacheInterface {
            public:
                typedef enum SharedVeriableDomain {
                    SVD_INPUT,
                    SVD_OUTPUT,
                    SVD_CUSTOM
                }SharedVeriableDomain;
                
                //! Return the value object for the domain and the string key
                /*!
                 \param domain a domain identified by a value of @IOCAttributeSharedCache::SharedVeriableDomain
                 \key_name a name that identify the variable
                 */
                virtual ValueSetting *getVariableValue(SharedVeriableDomain domain, const char *variable_name) = 0;
                
                //! Return the value object for the domain and the index of the variable
                virtual ValueSetting *getVariableValue(SharedVeriableDomain domain, VariableIndexType variable_index) = 0;
                
                //! Set the value for a determinated variable in a determinate domain
                virtual void setVariableValueForKey(SharedVeriableDomain domain, const char *variable_name, void * value, uint32_t size) = 0;
                
                //! Get the index of the changed attribute
                virtual void getChangedVariableIndex(SharedVeriableDomain domain, std::vector<VariableIndexType>& changed_index) = 0;
                
                //! Return the names of all variabl einto a determinated domain
                virtual void getVariableNames(SharedVeriableDomain domain, std::vector<std::string>& names) = 0;
                
                //! Add a new variable
                virtual void addVariable(SharedCacheInterface::SharedVeriableDomain domain, std::string name, uint32_t max_size, chaos::DataType::DataType type) = 0;
                
                //! Get a specified type of shared domain
                /*!
                 Return a specified type of shared domain , identified by input parameter
                 \param the type of the domain to return
                 */
                virtual AttributeSetting& getSharedDomain(SharedVeriableDomain domain) = 0;
            };
            
            //-----------------------------------------------------------------------------------------------------------------------------
            
            /*!
             Convenient class for grupping toghether the three different
             cache for the attirbute (input, output, custom).
             */
            class IOCAttributeSharedCache : public chaos::utility::InizializableService, public SharedCacheInterface {
                AttributeSetting inputAttribute;
                AttributeSetting outputAttribute;
                AttributeSetting customAttribute;
                
            public:
                
                IOCAttributeSharedCache();
                ~IOCAttributeSharedCache();
                
                //! This field point to a custom memory shared by cu and all command
                void *customData;
                
                //! Initialize instance
                void init(void *initData) throw(chaos::CException);
                
                //! Deinit the implementation
                void deinit() throw(chaos::CException);
                
                //! Return the value object for the domain and the string key
                /*!
                 \param domain a domain identified by a value of @SharedCacheInterface::SharedVeriableDomain
                 \key_name a name that identify the variable
                 */
                ValueSetting *getVariableValue(SharedCacheInterface::SharedVeriableDomain domain, const char *variable_name);
                
                //! Return the value object for the domain and the index of the variable
                ValueSetting *getVariableValue(SharedCacheInterface::SharedVeriableDomain domain, VariableIndexType variable_index);
                
                //! Set the value for a determinated variable in a determinate domain
                void setVariableValueForKey(SharedCacheInterface::SharedVeriableDomain domain, const char *variable_name, void * value, uint32_t size);
                
                //! Get the index of the changed attribute
                void getChangedVariableIndex(SharedCacheInterface::SharedVeriableDomain domain, std::vector<VariableIndexType>& changed_index);
                
                //! Return the names of all variabl einto a determinated domain
                void getVariableNames(SharedCacheInterface::SharedVeriableDomain domain, std::vector<std::string>& names);
                
                //! Add a new variable
                void addVariable(SharedCacheInterface::SharedVeriableDomain domain, std::string name, uint32_t max_size, chaos::DataType::DataType type);
                
                //! Get a specified type of shared domain
                /*!
                 Return a specified type of shared domain , identified by input parameter
                 \param the type of the domain to return
                 */
                AttributeSetting& getSharedDomain(SharedCacheInterface::SharedVeriableDomain domain);
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__AttributeSetting__) */

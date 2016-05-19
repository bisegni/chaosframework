/*
 *	AttributeCache.h
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

#ifndef __CHAOSFramework__AttributeCache__
#define __CHAOSFramework__AttributeCache__

#include <map>

#include <boost/thread.hpp>
#include <boost/dynamic_bitset.hpp>

#include <chaos/common/data/cache/AttributeValue.h>
#include <chaos/common/utility/InizializableService.h>

namespace chaos{
    namespace common {
        namespace data {
            namespace cache {

                    //! the range of index
                typedef  uint16_t VariableIndexType;

                    //! this class is a set of key with a ValueSetting class associated
                /*!
                 This class collect a set on key with the repsective ValueSetting creating a domain o
                 values.
                 */
                class AttributeCache :
                public utility::InizializableService {
                        //!global index for this set
                    VariableIndexType index;

                    boost::dynamic_bitset<BitBlockDimension> bitmapChangedAttribute;

                    std::map<std::string, VariableIndexType> mapAttributeNameIndex;
                    std::map<VariableIndexType, boost::shared_ptr<AttributeValue> > mapAttributeIndex;


                public:
                    boost::shared_ptr<boost::shared_mutex>	mutex;

                    AttributeCache();

                    ~AttributeCache();

                    void addAttribute(const std::string& name,
                                      uint32_t size,
                                      chaos::DataType::DataType type);

                    void setValueForAttribute(VariableIndexType n,
                                              const void * value,
                                              uint32_t size);

                    void setValueForAttribute(const std::string& name,
                                              const void * value,
                                              uint32_t size);

                    bool hasName(const std::string& name);

                    VariableIndexType getIndexForName(const std::string& name );

                        //! get all attribute name in this set
                    void getAttributeNames(std::vector<std::string>& names);

                        //! Initialize instance
                    void init(void *initData) throw(chaos::CException);

                        //! Deinit the implementation
                    void deinit() throw(chaos::CException);

                        //! return the changed id into the vector
                    void getChangedIndex(std::vector<VariableIndexType>& changed_index);

                        //! get the ValueSetting for the index
                    AttributeValue *getValueSettingForIndex(VariableIndexType index);

                        //! get the ValueSetting for the index
                    AttributeValue *getValueSettingByName(const std::string& name);

                        //!fill the CDataWrapper representig the set
                    void fillDataWrapper(CDataWrapper& data_wrapper);

                        //! return the number of the attribute into the domain
                    VariableIndexType getNumberOfAttributes();
                    
                        //! reset the chagned index array
                    void resetChangedIndex();
                    
                        //! mark all attribute as changed
                    void markAllAsChanged();
                    
                        //! return true if some attribute has change it's value
                    bool hasChanged();
                    
                        //! set new size on attribute by index
                    bool setNewSize(VariableIndexType attribute_index,
                                    uint32_t new_size,
                                    bool clear_mem = false);
                        //! set new size on attribute by name
                    bool setNewSize(const std::string& attribute_name,
                                    uint32_t new_size,
                                    bool clear_mem);
                        //! check if an attribute is present
                    bool hasAttribute(const std::string& attribute_name);
                };
            }
        }
    }
}
#endif /* defined(__CHAOSFramework__AttributeCache__) */

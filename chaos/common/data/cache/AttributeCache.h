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
                
                CHAOS_DEFINE_VECTOR_FOR_TYPE(ChaosSharedPtr<AttributeValue>, AttributeValueVector);
                
                //! this class is a set of key with a ValueSetting class associated
                /*!
                 This class collect a set on key with the repsective ValueSetting creating a domain o
                 values.
                 */
                class AttributeCache {
                    //!global index for this set
                    VariableIndexType index;
                    
                    boost::dynamic_bitset<BitBlockDimension> bitmapChangedAttribute;
                    
                    std::map<std::string, VariableIndexType> mapAttributeNameIndex;
                    
                    //attribute vector
                    AttributeValueVector vector_attribute_value;
                    
                    void copyAttribute(AttributeValue& av);
                public:
                    mutable ChaosSharedPtr<boost::shared_mutex>	mutex;
                    
                    AttributeCache();
                    
                    ~AttributeCache();
                    
                    void addAttribute(const std::string& name,
                                      uint32_t size,
                                      chaos::DataType::DataType type,
                                      const std::vector<chaos::DataType::BinarySubtype>& sub_type = std::vector<chaos::DataType::BinarySubtype>());
                    
                    void setValueForAttribute(VariableIndexType n,
                                              const void * value,
                                              uint32_t size);
                    
                    void setValueForAttribute(VariableIndexType n, CDataWrapper& value);
                    
                    void setValueForAttribute(const std::string& name,
                                              const void * value,
                                              uint32_t size);
                    
                    bool hasName(const std::string& name);
                    
                    VariableIndexType getIndexForName(const std::string& name );
                    
                    //! get all attribute name in this set
                    void getAttributeNames(std::vector<std::string>& names);
                    
                    //! Deinit the implementation
                    void reset();
                    
                    //! return the changed id into the vector
                    void getChangedIndex(std::vector<VariableIndexType>& changed_index);
                    
                    //! get the ValueSetting for the index
                    AttributeValue *getValueSettingForIndex(VariableIndexType index);
                    
                    //! get the ValueSetting for the index
                    AttributeValue *getValueSettingByName(const std::string& name);
                    
                    //! return the number of the attribute into the domain
                    VariableIndexType getNumberOfAttributes();
                    
                    //! reset the chagned index array
                    void resetChangedIndex();
                    
                    //! mark all attribute as changed
                    void markAllAsChanged();
                    
                    //! return true if some attribute has change it's value
                    bool hasChanged() const;
                    
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
                    
                    void writeAttributeToCDataWrapper(const std::string& attribute_name,
                                                      CDataWrapper& dest_dw);
                    //!export all attribute into a cdata wrapper
                    /*!
                     th einsertion into the cdata wrapper follow the creation order of the
                     attribute
                     */
                    void exportToCDataWrapper(CDataWrapper& dest_dw) const;
                    
                    void copyToAttributeCache(AttributeCache& dest_attribute_cache);
                };
                
                
                class SharedCacheLockDomain {
                protected:
                    ChaosSharedPtr<boost::shared_mutex>	mutex;
                public:
                    SharedCacheLockDomain(ChaosSharedPtr<boost::shared_mutex>& _mutex);
                    
                    virtual ~SharedCacheLockDomain();
                    
                    virtual void lock() = 0;
                    
                    virtual void unlock() = 0;
                };
                
                class WriteSharedCacheLockDomain : public SharedCacheLockDomain {
                    boost::unique_lock<boost::shared_mutex> w_lock;
                public:
                    WriteSharedCacheLockDomain(ChaosSharedPtr<boost::shared_mutex>& _mutex, bool lock = false);
                    
                    ~WriteSharedCacheLockDomain();
                    
                    void lock();
                    
                    void unlock();
                };
                
                class ReadSharedCacheLockDomain : public SharedCacheLockDomain {
                    boost::shared_lock<boost::shared_mutex> r_lock;
                public:
                    ReadSharedCacheLockDomain(ChaosSharedPtr<boost::shared_mutex>& _mutex, bool lock = false);
                    
                    ~ReadSharedCacheLockDomain();
                    
                    void lock();
                    
                    void unlock();
                };
                
            }
        }
    }
}
#endif /* defined(__CHAOSFramework__AttributeCache__) */

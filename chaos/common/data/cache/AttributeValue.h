/*
 *	AttributesValue.h
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


#ifndef __CHAOSFramework__AttributeValue__
#define __CHAOSFramework__AttributeValue__

#include <stdint.h>

#include <boost/dynamic_bitset.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataVariant.h>

namespace chaos{
    namespace common {
        namespace data {

            class CDataWrapper;

            namespace cache {

                    //! the dimensio of the block for the boost::dynamic_bitset class
                typedef  uint8_t BitBlockDimension;

                    //! manage the update of a value
                /*!
                 A AttributeValue is a class that help to understand when a value is changed and updated
                 */
                struct AttributeValue {
                    friend class AttributeSetting;
                    const std::string					name;
                        //! the index of this value
                    const uint32_t                      index;

                        //! the size of value
                    uint32_t							size;

                        //! the size of the buffer
                    uint32_t							buf_size;
                        //! is the datatype that represent the value
                    const chaos::DataType::DataType     type;

                        //!main buffer
                    void								*value_buffer;

                        //global index bitmap for infom that this value(using index) has been changed
                    boost::dynamic_bitset<BitBlockDimension> * sharedBitmapChangedAttribute;

                        //!pirvate constructor
                    AttributeValue(const std::string& _name,
                                   uint32_t _index,
                                   uint32_t _size,
                                   chaos::DataType::DataType type);

                        //!private destrucotr
                    ~AttributeValue();

                        //! change the size of the attribute value
                    bool setNewSize(uint32_t _new_size,
                                    bool clear_mem = false);

                        //! set a new value in buffer
                    /*!
                     the memory on value_ptr is copied on the internal memory buffer
                     \param value_ptr the memory ptr that contains the new value to copy into internal memory
                     \param value_size the sie of the new value
                     */
                    bool setValue(const void* value_ptr,
                                  uint32_t value_size,
                                  bool tag_has_changed = true);
                    
                    bool setValue(const CDataVariant& attribute_value,
                                  bool tag_has_changed = true);
                    
                        //! marck attribute as changed
                    void markAsChanged();

                        //! marck attribute as changed
                    void markAsUnchanged();

                        //! the value is returned has handle because the pointer can change it size ans so
                        //! the pointer can be relocated
                    template<typename T>
                    inline T* getValuePtr() {
                        return reinterpret_cast<T*>(value_buffer);
                    }
                    
                        //! return the buffer as cdatawrapper
                    /*!
                     This will work only if the contained data is a cdata buffer serialization
                     */
                    CDataWrapper *getValueAsCDatawrapperPtr(bool from_json = false);
                    
                        //! write to data wrapper as key value
                    void writeToCDataWrapper(CDataWrapper& data_wrapper);

                        //! 
                    std::string toString();
                    
                        //! check if attribute buffer is good
                    bool isGood();
                    
                        //!return value as CDataVariant
                    CDataVariant getAsVariant();

                private:
                    inline void reallignPointer();
                    
                    inline bool grow(uint32_t value_size);
                };
            }
        }
    }
}

#endif /* defined(__CHAOSFramework__AttributeValue__) */

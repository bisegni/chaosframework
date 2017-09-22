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


#ifndef __CHAOSFramework__AttributeValue__
#define __CHAOSFramework__AttributeValue__

#include <stdint.h>

#include <boost/dynamic_bitset.hpp>

#include <chaos/common/chaos_constants.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/data/CDataWrapper.h>

namespace chaos{
    namespace common {
        namespace data {

            class CDataWrapper;
            namespace cache {
                
                class AttributeCache;
            	namespace spec {
            	 template<typename T>
            	       inline  T* getValuePtr(void* value_buffer,CDataWrapper *cd) {
            	                   return reinterpret_cast<T*>(value_buffer);
            	           }
            	   template<>
            	     inline  CDataWrapper* getValuePtr<CDataWrapper>(void* value_buffer,CDataWrapper *cd){
            	                   return cd;
            	               }
            	}
                    //! the dimensio of the block for the boost::dynamic_bitset class
                typedef  uint8_t BitBlockDimension;

                    //! manage the update of a value
                /*!
                 A AttributeValue is a class that help to understand when a value is changed and updated
                 */
                struct AttributeValue {
                    friend class AttributeCache;
                    const std::string					name;
                        //! the index of this value
                    const uint32_t                      index;

                        //! the size of value
                    uint32_t							size;

                        //! the size of the buffer
                    uint32_t							buf_size;
                        //! is the datatype that represent the value
                    const chaos::DataType::DataType     type;
                    
                        //! subtype
                    std::vector<chaos::DataType::BinarySubtype> sub_type;
                    
                    //!main buffer
                    void								*value_buffer;
                    CDataWrapper						cdvalue;

                        //global index bitmap for infom that this value(using index) has been changed
                    boost::dynamic_bitset<BitBlockDimension> * sharedBitmapChangedAttribute;

                        //!pirvate constructor
                    AttributeValue(const std::string& _name,
                                   uint32_t _index,
                                   uint32_t _size,
                                   chaos::DataType::DataType type,
                                   const std::vector<chaos::DataType::BinarySubtype>& _sub_type);

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
                    
                    bool setValue(CDataWrapper& attribute_value,
                                  bool tag_has_changed = true);
                    bool setValue(const CDataVariant& attribute_value,
                                                      bool tag_has_changed = true);
                    bool setStringValue(const std::string& value,
                                        bool tag_has_changed = true,
                                        bool enlarge_memory = false);
                    
                        //! marck attribute as changed
                    void markAsChanged();

                        //! marck attribute as changed
                    void markAsUnchanged();

                        //! the value is returned has handle because the pointer can change it size ans so
                        //! the pointer can be relocated
                    template<typename T>
                     inline T* getValuePtr() {
                        //return reinterpret_cast<T*>(value_buffer);
                    	return spec::getValuePtr<T>(value_buffer,&cdvalue);
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
                    
                    //!
                    std::string toString(int double_precision);
                    
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

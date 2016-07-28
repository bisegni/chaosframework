/*
 *	TempaltedDataHelper.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_EB44C3EB_D602_48B6_A57E_FBF4555A4C6A_TempaltedDataHelper_h
#define __CHAOSFramework_EB44C3EB_D602_48B6_A57E_FBF4555A4C6A_TempaltedDataHelper_h

#include <chaos/common/data/CDataWrapper.h>
#if ! defined(BOOST_NO_TYPEID)
#define GET_TYPE_NAME(T) typeid(T).name();
#else
#include <boost/type_index.hpp>
#define GET_TYPE_NAME(T) boost::typeindex::type_id<T>().pretty_name();
#endif

#include <typeinfo>
#include <vector>

namespace chaos {
    namespace common {
        namespace data {
            
            template<typename T>
            struct ReferenceSDWrapper {
            private:
                T private_deafault_data;
            public:
                T& data;
                ReferenceSDWrapper():
                data(private_deafault_data){}
            };
            
            template<typename T>
            struct CopySDWrapper {
                T data;
            };
            
            //! serializer deserializer wrap for class that embed data to transfer with cdata wrapper
            template<typename T, typename W = CopySDWrapper<T> >
            class TemplatedDataSDWrapper {
                W wrapper;
                //W wrapper;
            public:
                //!constructor with the default container
                TemplatedDataSDWrapper(){}
                TemplatedDataSDWrapper(const T& data_src){wrapper.data = data_src;}
                TemplatedDataSDWrapper(chaos::common::data::CDataWrapper *serialized_data){}
                virtual ~TemplatedDataSDWrapper(){};
                
                //!deserialize encoded data in container
                virtual void deserialize(chaos::common::data::CDataWrapper *serialized_data) = 0;
                
                //!serialize the container
                virtual std::auto_ptr<chaos::common::data::CDataWrapper> serialize() = 0;
                
                //!return the container
                T& dataWrapped(){
                    return wrapper.data;
                }
                
                //!assign operation overload with wrapper
                T& operator()() {
                    return wrapper.data;
                }
                
                const T& dataWrapped() const {
                    return wrapper.data;
                }
                
                //!assign operation overload with contained data
                T& operator=(T const &rhs) {
                    return (wrapper.data = rhs);
                }
                
                //!assign operation overload with wrapper
                TemplatedDataSDWrapper<T>& operator=(TemplatedDataSDWrapper<T> const &rhs) {
                    wrapper.data = rhs.wrapper.data;
                    return *this;
                }
            };
            
            
#define CHAOS_SD_WRAPPER_NAME(x)  x ## SDWrapper
#define CHAOS_SD_WRAPPER_NAME_VAR(x, v)  CHAOS_SD_WRAPPER_NAME(x) v
            
#define CHAOS_OPEN_SDWRAPPER(x)\
template<typename W = chaos::common::data::CopySDWrapper<x> >\
class x ## SDWrapper:\
public chaos::common::data::TemplatedDataSDWrapper<x, W> {\
typedef chaos::common::data::TemplatedDataSDWrapper<x, W> Subclass; public:\
CHAOS_SD_WRAPPER_NAME(x)():\
Subclass(){}\
CHAOS_SD_WRAPPER_NAME(x)(const x& copy_source):\
Subclass(copy_source){}\
CHAOS_SD_WRAPPER_NAME(x)(chaos::common::data::CDataWrapper *serialized_data):\
Subclass(serialized_data){deserialize(serialized_data);}
            
#define CHAOS_CLOSE_SDWRAPPER() };
            
#define CHAOS_DEFINE_SDWRAPPER_CONSTRUCTOR(x)\
CHAOS_SD_WRAPPER_NAME(x)():\
chaos::common::data::TemplatedDataSDWrapper<x, W>(){}\
CHAOS_SD_WRAPPER_NAME(x)(const x& copy_source):\
chaos::common::data::TemplatedDataSDWrapper<x, W>(copy_source){}\
CHAOS_SD_WRAPPER_NAME(x)(chaos::common::data::CDataWrapper *serialized_data):\
chaos::common::data::TemplatedDataSDWrapper<x, W>(serialized_data){deserialize(serialized_data);}

#define CHAOS_DEFINE_TEMPLATED_SDWRAPPER_ALIAS_AND_WRAPPER(wrp, a, x)\
typedef chaos::common::data::TemplatedDataSDWrapper<x,w> a ## SDWrapperSubclass;\
template<typename w= wrp>\
class a ## SDWrapper:\
public a ## SDWrapperSubclass
            
#define CHAOS_DEFINE_TEMPLATED_SDWRAPPER_ALIAS(a,x)\
typedef chaos::common::data::TemplatedDataSDWrapper<x> a ## SDWrapperSubclass;\
class a ## SDWrapper:\
public a ## SDWrapperSubclass
            
#define CHAOS_DEFINE_TEMPLATED_SDWRAPPER_CLASS(x)\
CHAOS_DEFINE_TEMPLATED_SDWRAPPER_ALIAS(x,x)
            
#define CHAOS_DEFINE_TEMPLATED_SDWRAPPER_CLASS_W(w,x)\
CHAOS_DEFINE_TEMPLATED_SDWRAPPER_ALIAS(w,x,x)
            
            
#define CHAOS_DECLARE_SD_WRAPPER_CONSTRUCTOR(x)\
CHAOS_DEFINE_SD_WRAPPER(x)():\
chaos::common::data::TemplatedDataSDWrapper<x>(){}\
CHAOS_SD_WRAPPER_NAME(x)(const Dataset& copy_source):\
chaos::common::data::TemplatedDataSDWrapper<x>(copy_source){}\
CHAOS_SD_WRAPPER_NAME(x)(chaos::common::data::CDataWrapper *serialized_data):\
chaos::common::data::TemplatedDataSDWrapper<x>(serialized_data){deserialize(serialized_data);}\

            
            
            
            template<typename T,
            typename DW>
            class TemplatedDataListWrapper {
                const std::string instance_serialization_key;
                static const std::string master_serialization_key;
            public:
                typedef std::vector< T >                              WrapList;
                typedef typename std::vector< T >::iterator           WrapListIterator;
                typedef typename std::vector< T >::const_iterator     WrapListConstIterator;
                
            private:
                WrapList data_list;
            public:
                TemplatedDataListWrapper():
                instance_serialization_key(master_serialization_key){}
                
                TemplatedDataListWrapper(const std::string& serialization_postfix):
                instance_serialization_key(master_serialization_key+serialization_postfix) {
                    
                }
                
                ~TemplatedDataListWrapper() {}
                
                WrapList& dataListWrapped() {
                    return data_list;
                }
                
                T& operator()() {
                    return data_list;
                }
                
                size_t size() const {
                    return data_list.size();
                }
                
                WrapListIterator begin() {
                    return data_list.begin();
                }
                
                WrapListConstIterator begin() const {
                    return data_list.begin();
                }
                
                WrapListIterator end() {
                    return data_list.end();
                }
                
                WrapListConstIterator end() const {
                    return data_list.end();
                }
                
                void add(const T& element) {
                    data_list.push_back(element);
                }
                
                void add(chaos::common::data::CDataWrapper *element_serailized) {
                    DW serializer_wrap(element_serailized);
                    data_list.push_back(serializer_wrap.dataWrapped());
                }
                
                T& operator[](const int& i) {
                    return data_list[i];
                }
                
                const T& operator[](const int& i) const {
                    return data_list[i];
                }
                //!assign operation overload with wrapper
                TemplatedDataListWrapper<T,DW>& operator=(TemplatedDataListWrapper<T,DW> const &rhs) {
                    data_list = rhs.data_list;
                    return *this;
                }
                
                //!deserialize encoded data in list
                void deserialize(chaos::common::data::CDataWrapper *serialized_data,
                                 bool remove_old_element = true) {
                    DW serializer_wrap;
                    if(!(serialized_data &&
                         serialized_data->hasKey(instance_serialization_key) &&
                         serialized_data->isVectorValue(instance_serialization_key))) return;
                    
                    if(remove_old_element) {
                        data_list.clear();
                    }
                    
                    //we can deserialize data
                    std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(instance_serialization_key));
                    for(int idx = 0;
                        idx < serialized_array->size();
                        idx++) {
                        std::auto_ptr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                        serializer_wrap.deserialize(element.get());
                        add(serializer_wrap.dataWrapped());
                    }
                }
                
                //!serialize the list
                std::auto_ptr<chaos::common::data::CDataWrapper> serialize() {
                    DW serializer_wrap;
                    std::auto_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                    for(WrapListIterator it = TemplatedDataListWrapper<T,DW>::begin(),
                        end = TemplatedDataListWrapper<T,DW>::end();
                        it != end;
                        it++) {
                        serializer_wrap = *it;
                        result->appendCDataWrapperToArray(*serializer_wrap.serialize());
                    }
                    result->finalizeArrayForKey(instance_serialization_key);
                    return result;
                }
                
            };
            
            template<typename T, typename DW>
            const  std::string TemplatedDataListWrapper<T,DW>::master_serialization_key = GET_TYPE_NAME(T);
            
#define CHAOS_DEFINE_SD_LIST_WRAPPER(x, serializer_wrapper)  chaos::common::data::TemplatedDataListWrapper<x,serializer_wrapper>
            
#define CHAOS_DEFINE_TYPE_FOR_SD_LIST_WRAPPER(x, serializer_wrapper, type_name)  typedef CHAOS_DEFINE_SD_LIST_WRAPPER(x, serializer_wrapper) type_name
            
#define CHAOS_DECLARE_SD_LIST_WRAPPER_VAR(x, v)  CHAOS_DEFINE_SD_LIST_WRAPPER(x) v
        }
    }
}

#endif /* __CHAOSFramework_EB44C3EB_D602_48B6_A57E_FBF4555A4C6A_TempaltedDataHelper_h */

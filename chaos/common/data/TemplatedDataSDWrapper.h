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
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataWrapper.h>
#if ! defined(BOOST_NO_TYPEID)
#define GET_TYPE_NAME(T) typeid(T).name();
#else
#include <boost/type_index.hpp>
#define GET_TYPE_NAME(T) boost::typeindex::type_id<T>().pretty_name();
#endif


#define CHAOS_DATA_WRAPPER_REFERENCE_AUTO_PTR(x, default_param)\
ChaosSharedPtr< chaos::common::data::DataWrapperReference<x> >(new chaos::common::data::DataWrapperReference<x>(default_param))

#define CHAOS_SD_WRAPPER_DEFAULT_PARAMETER(x)\
ChaosSharedPtr< chaos::common::data::DataWrapperReference<x> > _data = ChaosSharedPtr< chaos::common::data::DataWrapperReference<x> >(new chaos::common::data::DataWrapperCopy<x>())

#include <typeinfo>
#include <vector>

namespace chaos {
    namespace common {
        namespace data {
            
            //strcut that wrapper around a reference to a variable types
            template <typename T>
            struct DataWrapperReference {
                T& data;
                
                //!constructor with referenced objec
                DataWrapperReference(T& referenced_instance):
                data(referenced_instance){}
                
                //!return the container
                T& dataWrapped(){
                    return data;
                }
                
                //!assign operation overload with wrapper
                T& operator()() {
                    return data;
                }
                
                const T& dataWrapped() const {
                    return data;
                }
                
                //!assign operation overload with contained data
                T& operator=(const T &rhs) {
                    return data = rhs;
                }
                
                DataWrapperReference<T>& operator=(const DataWrapperReference<T> &rhs) {
                    data = rhs.data;
                    return *this;
                }
            };
            
            //! class that wrap a copy of a veriable types
            template <typename T>
            struct DataWrapperCopy:
            public DataWrapperReference<T> {
            private:
                //!private copy for empty constructor
                T private_copy;
            public:
                DataWrapperCopy():
                DataWrapperReference<T>(private_copy){}
                ~DataWrapperCopy(){}
                
                //!assign operation overload with contained data
                T& operator=(const T &rhs) {
                    return private_copy = rhs;
                }
                
                DataWrapperCopy<T>& operator=(const DataWrapperCopy<T> &rhs) {
                    private_copy = rhs.private_copy;
                    return *this;
                }
            };

            
            //! serializer deserializer wrap for class that embed data to transfer with cdata wrapper
            template<typename T>
            class TemplatedDataSDWrapper {
                //pointer todata wrapper

            	ChaosSharedPtr < DataWrapperReference<T> > wrapper;
            public:
                //!constructor with the default container
                TemplatedDataSDWrapper(ChaosSharedPtr< DataWrapperReference<T> >& _wrapper):
                wrapper(_wrapper){}
                
                TemplatedDataSDWrapper(const T& copy_src,
                		ChaosSharedPtr< DataWrapperReference<T> >& _wrapper):
                wrapper(_wrapper){
                    (*wrapper)() = copy_src;
                }
                
                //destructor
                virtual ~TemplatedDataSDWrapper(){};
                
                //!deserialize encoded data in container
                virtual void deserialize(chaos::common::data::CDataWrapper *serialized_data) = 0;
                
                //!serialize the container
                virtual ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() = 0;
                
                //!return the container
                T& dataWrapped(){
                    return (*wrapper)();
                }
                
                //!assign operation overload with wrapper
                T& operator()() {
                    return (*wrapper)();
                }
                
                const T& dataWrapped() const {
                    return (*wrapper)();
                }
                
                //!assign operation overload with contained data
                T& operator=(const T &rhs) {
                    return (*wrapper)() = rhs;
                }
                
                DataWrapperReference<T>& operator=(const DataWrapperReference<T> &rhs) {
                    wrapper = rhs.wrapper;
                    return *this;
                }
            };
            
            
#define CHAOS_SD_WRAPPER_NAME(x)  x ## SDWrapper
            
#define CHAOS_SD_WRAPPER_NAME_VAR(x, v)  CHAOS_SD_WRAPPER_NAME(x) v
            
            
#define CHAOS_SD_WRAPPER_DEFAULT_CONSTRUCTOR(x)\
CHAOS_SD_WRAPPER_NAME(x)(CHAOS_SD_WRAPPER_DEFAULT_PARAMETER(x))
            
#define CHAOS_OPEN_SDWRAPPER(x)\
struct CHAOS_SD_WRAPPER_NAME(x):\
public chaos::common::data::TemplatedDataSDWrapper<x> {\
typedef chaos::common::data::TemplatedDataSDWrapper<x> Subclass; public:\
CHAOS_SD_WRAPPER_DEFAULT_CONSTRUCTOR(x):\
Subclass(_data){}\
CHAOS_SD_WRAPPER_NAME(x)(chaos::common::data::CDataWrapper *serialized_data, CHAOS_SD_WRAPPER_DEFAULT_PARAMETER(x)):\
Subclass(_data){deserialize(serialized_data);}\
CHAOS_SD_WRAPPER_NAME(x)(const x& copy_source, CHAOS_SD_WRAPPER_DEFAULT_PARAMETER(x)):\
Subclass(copy_source, _data){}

            
#define CHAOS_CLOSE_SDWRAPPER() };
            
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
                    ChaosUniquePtr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(instance_serialization_key));
                    for(int idx = 0;
                        idx < serialized_array->size();
                        idx++) {
                        ChaosUniquePtr<chaos::common::data::CDataWrapper> element(serialized_array->getCDataWrapperElementAtIndex(idx));
                        serializer_wrap.deserialize(element.get());
                        add(serializer_wrap.dataWrapped());
                    }
                }
                
                //!serialize the list
                ChaosUniquePtr<chaos::common::data::CDataWrapper> serialize() {
                    DW serializer_wrap;
                    ChaosUniquePtr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
                    for(WrapListIterator it = TemplatedDataListWrapper<T,DW>::begin(),
                        end = TemplatedDataListWrapper<T,DW>::end();
                        it != end;
                        it++) {
                        serializer_wrap() = *it;
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

/*
 *	ObjectInstancer.h
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#ifndef CHAOSFramework_ObjectInstancer_h
#define CHAOSFramework_ObjectInstancer_h

#include <map>

#include <boost/thread.hpp>

namespace chaos {
    namespace common {
        namespace utility {
            
            /*!
             Templated interface that give rule for the the instantiation of a class.
             */
            template <typename R >
            class ObjectInstancer {
            public:
                virtual ~ObjectInstancer(){};
                virtual R* getInstance() = 0;
            };
            
			/*!
             Templated interface that give rule for the the instantiation of a class with one param constructor.
             */
			template <typename R, typename p1 >
            class ObjectInstancerP1 {
            public:
                virtual ~ObjectInstancerP1(){};
                virtual R* getInstance(p1 _p1) = 0;
            };
			
			/*!
             Templated interface that give rule for the the instantiation of a class with two param constructor.
             */
			template <typename R, typename p1, typename p2 >
            class ObjectInstancerP2 {
            public:
                virtual ~ObjectInstancerP2(){};
                virtual R* getInstance(p1 _p1, p2 _p2) = 0;
            };
			
			/*!
             Templated interface that give rule for the the instantiation of a class with three param constructor.
             */
			template <typename R, typename p1, typename p2, typename p3 >
            class ObjectInstancerP3 {
            public:
                virtual ~ObjectInstancerP3(){};
                virtual R* getInstance(p1 _p1, p2 _p2, p3 _p3) = 0;
            };
			
            /*!
             Templated interface that regolate the instantiation of a class.
             */
            template <typename S , typename B >
            class NestedObjectInstancer : public ObjectInstancer<B> {
                ObjectInstancer<S> *instancer;
            public:
                NestedObjectInstancer(ObjectInstancer<S> *_instancer):instancer(_instancer){};
                virtual ~NestedObjectInstancer(){
                    if(instancer) {
                        delete instancer;
                    }
                };
                virtual B* getInstance() {
                    return instancer->getInstance();
                }
            };
            
            /*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R >
            class TypedObjectInstancer : public ObjectInstancer<R> {
            public:
                R* getInstance() {
                    return new T();
                }
            };
			
			/*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1>
            class TypedObjectInstancerP1 : public ObjectInstancerP1<R, p1> {
            public:
                R* getInstance(p1 _p1) {
                    return new T(_p1);
                }
            };
			
			/*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1, typename p2 >
            class TypedObjectInstancerP2 : public ObjectInstancerP2<R, p1, p2> {
            public:
                R* getInstance(p1 _p1, p2 _p2) {
                    return new T(_p1, _p2);
                }
            };
			
			/*!
             Templated class that permit to instantiate the superclas of
             a base class. This class permit to check this rule at compiletime
             */
            template <typename T, typename R, typename p1, typename p2, typename p3 >
            class TypedObjectInstancerP3 : public ObjectInstancerP3<R, p1, p2, p3> {
            public:
                R* getInstance(p1 _p1, p2 _p2, p3 _p3) {
                    return new T(_p1, _p2, _p3);
                }
            };
			
			#define INSTANCER(ImplClass, BaseClass) chaos::common::utility::TypedObjectInstancer<ImplClass, BaseClass>
			#define ALLOCATE_INSTANCER(ImplClass, BaseClass) new INSTANCER(ImplClass, BaseClass)
			
			#define INSTANCER_P1(ImplClass, BaseClass, p1) chaos::common::utility::TypedObjectInstancerP1<ImplClass, BaseClass, p1>
			#define ALLOCATE_INSTANCER_P1(ImplClass, BaseClass, p1) new INSTANCER_P1(ImplClass, BaseClass, p1)

			#define INSTANCER_P2(ImplClass, BaseClass, p1, p2) chaos::common::utility::TypedObjectInstancerP2<ImplClass, BaseClass, p1, p2>
			#define ALLOCATE_INSTANCER_P2(ImplClass, BaseClass, p1, p2) new INSTANCER_P2(ImplClass, BaseClass, p1, p2)()

			#define INSTANCER_P3(ImplClass, BaseClass, p1, p2, p3) chaos::common::utility::TypedObjectInstancerP3<ImplClass, BaseClass, p1, p2, p3>
			#define ALLOCATE_INSTANCER_P3(ImplClass, BaseClass, p1, p2, p3) new INSTANCER_P3(ImplClass, BaseClass, p1, p2, p3)
			
			typedef boost::shared_mutex						InstancerReadWriteMutexClass;
			typedef boost::unique_lock<boost::shared_mutex>	InstancerContainerWriteLockClass;
			typedef boost::shared_lock<boost::shared_mutex> InstancerContainerReadLockClass;
			template <typename T>
			class InstancerContainer {
				
				InstancerReadWriteMutexClass m;
				
				std::map<std::string, ObjectInstancer<T> * > map_instancers;
				
			public:
				//! Constructor
				InstancerContainer() {
					
				}
				//! Destructor
				virtual ~InstancerContainer() {
					clearAllInstancer();
				}
				
				//! Clear all instancer
				void clearAllInstancer() {
					for(typename std::map<std::string, ObjectInstancer<T>* >::iterator it = map_instancers.begin();
						it != map_instancers.end();
						it++) {
						if(it->second) delete(it->second);
					}
				}
				
				//!
				void addInstancer(std::string key, utility::ObjectInstancer<T> *instancer) {
					InstancerContainerWriteLockClass lock(m);
					if(map_instancers.count(key)) return;
					map_instancers.insert(make_pair(key, instancer));
				}
				//!
				template<typename I>
				void addInstancer(std::string key) {
					InstancerContainerWriteLockClass lock(m);
					if(map_instancers.count(key)) return;
					map_instancers.insert(make_pair(key, ALLOCATE_INSTANCER(I,T)));
				}
				
				void removeInstancerByKey(std::string key) {
					InstancerContainerWriteLockClass lock(m);
					if(!map_instancers.count(key)) return;
					map_instancers.erase(key);
				}
				
				T* getInstanceByKey(std::string key) {
					InstancerContainerReadLockClass lock(m);
					if(!map_instancers.count(key)) return NULL;
					return map_instancers[key]->getInstance();
				}
			};
        }
    }
}

#endif

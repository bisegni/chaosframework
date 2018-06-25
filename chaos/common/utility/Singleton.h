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

#ifndef Singleton_H
#define Singleton_H

#include <boost/thread/once.hpp>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
//#include <chaos/common/global.h>

namespace chaos {
    namespace common {
        namespace utility {
            
#define SUBCLASS_AS_SINGLETON(ClassName) \
public Singleton<ClassName>{\
friend class Singleton<ClassName>;
            
#define DEFINE_CLASS_AS_SINGLETON(ClassName) \
class ClassName : public Singleton<ClassName>{\
friend class Singleton<ClassName>;
            
#define DEFINE_CLASS_AS_SINGLETON_WITH_OTHER_SUBCLASS(ClassName, Subclass) \
class ClassName : public Subclass, public Singleton<ClassName>{\
friend class Singleton<ClassName>;
            
            /*
             * Utility class for singleton find here: http://www.boostcookbook.com/Recipe:/1235044
             */
            template<class T>
            class Singleton:
            private boost::noncopyable {
            public:
                static T *getInstance() {
                    //static T singletonInstance;
                    call_once(_singletonInit, flag);
                    return t;
                }
                static void _singletonInit() {
                    if(t==0){
                        t= new T();
                    }
                }
                virtual ~Singleton(){if(t) delete t;t=0L;}
            protected:
                static T*  t;
                Singleton(){}
            private:
                static boost::once_flag flag;
                // Stop the compiler generating methods of copy the object
                Singleton(Singleton const& copy);            // Not Implemented
                Singleton& operator=(Singleton const& copy); // Not Implemented
            };
            
            template<class T> T* Singleton<T>::t=0L;
            template<class T> boost::once_flag Singleton<T>::flag = BOOST_ONCE_INIT;
        }
    }
}
#endif

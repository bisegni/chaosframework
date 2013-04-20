/*	
 *	Singleton.h
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

#ifndef Singleton_H
#define Singleton_H

#include <boost/utility.hpp>
#include <boost/thread/once.hpp>
#include <boost/shared_ptr.hpp>
#include <chaos/common/general/Configurable.h>

namespace chaos {
#define DEFINE_CLASS_AS_SINGLETON(ClassName) \
class ClassName : public Singleton<ClassName>{\
friend class Singleton<ClassName>;
    
#define DEFINE_CLASS_AS_SINGLETON_WITH_OTHER_SUBCLASS(ClassName, Subclass) \
class ClassName : public Subclass, public Singleton<ClassName>{\
friend class Singleton<ClassName>;
    using namespace boost;
    /*
     * Utility class for singleton find here: http://www.boostcookbook.com/Recipe:/1235044
     */
    template<class T>
    class Singleton : private noncopyable {
        
    public:
        static T* getInstance() {
                //static T singletonInstance;
            call_once(_singletonInit, flag);
            return t;
        }
        
        static void _singletonInit() {
            if(!t){
                t = new T();
            }
        }
        
    protected:
        ~Singleton() {}
        Singleton() {}
        
    private:
        static T *t;
        static boost::once_flag flag;
    };
        //template<class T> mutex Singleton<T>::stMutext;
    template<class T> T* Singleton<T>::t = 0L;
    template<class T> boost::once_flag Singleton<T>::flag = BOOST_ONCE_INIT;
}
#endif

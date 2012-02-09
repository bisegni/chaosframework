    //
    //  Singleton.h
    //  ChaosFramework
    //
    //  Created by Claudio Bisegni on 13/03/11.
    //  Copyright 2011 INFN. All rights reserved.
    //

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
            call_once(init, flag);
            return t;
        }
        
        static void init() {
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
    template<class T> once_flag Singleton<T>::flag = BOOST_ONCE_INIT;
}
#endif
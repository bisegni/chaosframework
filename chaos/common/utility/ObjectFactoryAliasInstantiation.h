//
//  ObjectFactoryAliasInstantiation.h
//  ChaosFramework
//
//  Created by Claudio Bisegni on 01/05/11.
//  Copyright 2011 INFN. All rights reserved.
//
#ifndef ObjectFactoryAliasInstantiation_H
#define ObjectFactoryAliasInstantiation_H

#include <string>
#include <boost/ptr_container/ptr_vector.hpp>
namespace chaos {
    using namespace boost;
    
    class ObjectFactory {
        public :
        ObjectFactory(const char *alias):sAlias(alias){};
        std::string sAlias;
        virtual void* createInstance() = 0;
    };
    
    /*
     Object factory with an alias
     */
    template <class T>
    class ObjectFactoryAliasInstantiation : public ObjectFactory{
    public:
        ObjectFactoryAliasInstantiation(const char *alias):ObjectFactory(alias){};
        virtual void* createInstance() { return (void*)new T(&sAlias); };
    };
}

#endif
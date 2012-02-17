/*	
 *	ObjectFactoryAliasInstantiation.h
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

/*	
 *	DispatcherRegister.h
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

#ifndef ChaosFramework_DispatcherRegister_h
#define ChaosFramework_DispatcherRegister_h

#include <chaos/common/utility/Singleton.h>
#include <string>

namespace chaos {
    
    using namespace std;
    
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
    class ObjectFactoryAliasInstantiation : public ObjectFactory {
    public:
        ObjectFactoryAliasInstantiation(const char *alias):ObjectFactory(alias){};
        virtual void* createInstance() { return (void*)new T(sAlias); };
    };

    /*
     Object factory with an alias
     */
    template <class T>
    class ObjectFactoryInstantiation : public ObjectFactory {
    public:
        ObjectFactoryInstantiation(const char *alias):ObjectFactory(alias){};
        virtual void* createInstance() { return (void*)new T(); };
    };
    
    /*
     Template class for keep track of aliast+factory for the object that has been
     registert for the type T
     */
    template <typename T>
    class ObjectFactoryRegister: public Singleton< ObjectFactoryRegister<T> > {
            //object factories map
        map<string, ObjectFactory*>  objectFacotoryList;
        
    public:
        /*
         Register the Abstract command factory for a Command associated with an alias
         */
        void registerObjectFactory(const char *alias, ObjectFactory *factoryPtr) {
                //register command into map
            string aliasString(alias);
            if(objectFacotoryList.count(aliasString) == 0){
                objectFacotoryList.insert(make_pair(aliasString, factoryPtr));
            }
            
        }
        /*
         Create a new instance for the command identified by input alias
         */
        T *getNewInstanceByName(const char *alias) {
            T *result=NULL;
            string aliasString(alias);
                //get the reference for the className
            ObjectFactory *acf = objectFacotoryList[aliasString];
                //return null ora create instance
            if(acf){
                result=(T*)acf->createInstance();
            }
            return result;
        }
    };
    
    /*
     Class for instantiate at startuptipe(statically) the object type
     */
    template <typename T>
    class ObjectInstancer {
    public:
        ObjectInstancer(ObjectFactory *commandFactory) {
            if(commandFactory){
                ObjectFactoryRegister<T>::getInstance()->registerObjectFactory((*commandFactory).sAlias.c_str(), commandFactory);
            }
        }
    };
    
#define MAKE_SERVICE_CLASS_CONSTRUCTOR(SERVICE_NAME) SERVICE_NAME(string *alias):NamedService(alias){};
 
#define MAKE_SERVICE_SUBCLASS_CONSTRUCTOR(SERVICE_NAME, BASE_SERVICE_NAME) SERVICE_NAME(string *alias):BASE_SERVICE_NAME(alias)
    /*
     Macro for help the Command Dispatcher classes registration
     */
#define REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(CMD_CLASS_NAME, BASE_CLASS_NAME) class CMD_CLASS_NAME;\
static const ObjectInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectInstancer(new ObjectFactoryAliasInstantiation<CMD_CLASS_NAME>(#CMD_CLASS_NAME));\
class CMD_CLASS_NAME : public BASE_CLASS_NAME
    
#define REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(t) \
    friend class chaos::ObjectFactoryAliasInstantiation<t>;
    
    
#define REGISTER_AND_DEFINE_NOALIAS_DERIVED_CLASS_FACTORY(CMD_CLASS_NAME, BASE_CLASS_NAME) class CMD_CLASS_NAME;\
static const ObjectInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectInstancer(new ObjectFactoryInstantiation<CMD_CLASS_NAME>(#CMD_CLASS_NAME));\
class CMD_CLASS_NAME : public BASE_CLASS_NAME
}
#endif

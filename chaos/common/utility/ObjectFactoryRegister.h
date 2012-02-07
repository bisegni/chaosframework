    //
    //  DispatcherRegister.h
    //  ChaosFramework
    //
    //  Created by Bisegni Claudio on 29/10/11.
    //  Copyright (c) 2011 INFN. All rights reserved.
    //

#ifndef ChaosFramework_DispatcherRegister_h
#define ChaosFramework_DispatcherRegister_h
#include <chaos/common/utility/ObjectFactoryAliasInstantiation.h>
#include <chaos/common/utility/Singleton.h>
#include <string>

namespace chaos {
    
    using namespace std;
    
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
            string aliasString = string(alias);
            if(objectFacotoryList.count(aliasString) == 0){
                objectFacotoryList.insert(make_pair(aliasString, factoryPtr));
            }
            
        }
        /*
         Create a new instance for the command identified by input alias
         */
        T *getNewInstanceByName(const char *alias) {
            T *result=NULL;
            string aliasString = string(alias);
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
    
    /*
     Macro for help the Command Dispatcher classes registration
     */
#define REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(CMD_CLASS_NAME, BASE_CLASS_NAME) class CMD_CLASS_NAME;\
static const ObjectInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectInstancer(new ObjectFactoryAliasInstantiation<CMD_CLASS_NAME>(#CMD_CLASS_NAME));\
class CMD_CLASS_NAME : public BASE_CLASS_NAME
}
#endif

/*
 *	DispatcherRegister.h
 *	!CHAOS
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

#include <map>
#include <string>
#include <vector>

namespace chaos {
	namespace common{
		namespace utility {
			class ObjectFactory {
			protected:
				std::string object_alias;
				public :
				ObjectFactory(const std::string & _object_alias):object_alias(_object_alias){};
				virtual void* createInstance() = 0;
				const std::string & getAlias() {
					return object_alias;
				}
			};
			
			/*
			 Object factory with an alias
			 */
			template <class T>
			class ObjectFactoryAliasInstantiation : public ObjectFactory {
			public:
				ObjectFactoryAliasInstantiation(const std::string & _object_alias):ObjectFactory(_object_alias){};
				virtual void* createInstance() { return (void*)new T(object_alias); };
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
				typedef std::map<std::string, ObjectFactory*>			ObjectFactoryMap;
				typedef std::map<std::string, ObjectFactory*>::iterator ObjectFactoryMapIterator;
				ObjectFactoryMap  object_factory_map;
				
			public:
				/*
				 Register the Abstract command factory for a Command associated with an alias
				 */
				void registerObjectFactory(const std::string & alias, ObjectFactory *factoryPtr) {
					//register command into map
					if(object_factory_map.count(alias) == 0){
						object_factory_map.insert(make_pair(alias, factoryPtr));
					}
					
				}
				
				//! return all registered name for the type T
				void getAllRegisteredName(std::vector<std::string>& registered_name_list) {
					for(ObjectFactoryMapIterator it = object_factory_map.begin();
						it != object_factory_map.end();
						it++) {
						registered_name_list.push_back(it->first);
					}
				}
				
				/*
				 Create a new instance for the command identified by input alias
				 */
				T *getNewInstanceByName(const std::string & alias) {
					T *result=NULL;
					//get the reference for the className
					ObjectFactory *acf = object_factory_map[alias];
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
			class ObjectFactoryInstancer {
			public:
				ObjectFactoryInstancer(ObjectFactory *commandFactory) {
				  //					LDBG_<<" instancing "<<commandFactory->getAlias();
					if(commandFactory){
						ObjectFactoryRegister<T>::getInstance()->registerObjectFactory(commandFactory->getAlias(), commandFactory);
					}
				}
				
			};
			
#define MAKE_SERVICE_CLASS_CONSTRUCTOR(SERVICE_NAME) SERVICE_NAME(std::string *alias):NamedService(alias){};
			
#define MAKE_SERVICE_SUBCLASS_CONSTRUCTOR(SERVICE_NAME, BASE_SERVICE_NAME) SERVICE_NAME(std::string *alias):BASE_SERVICE_NAME(alias)
			/*
			 Macro for help the Command Dispatcher classes registration
			 */
#define REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY(CMD_CLASS_NAME, BASE_CLASS_NAME) class CMD_CLASS_NAME;\
extern const chaos::common::utility::ObjectFactoryInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectFactoryInstancer(new chaos::common::utility::ObjectFactoryAliasInstantiation<CMD_CLASS_NAME>(#CMD_CLASS_NAME));\
class CMD_CLASS_NAME : public BASE_CLASS_NAME
			
#define DECLARE_CLASS_FACTORY(CMD_CLASS_NAME,BASE_CLASS_NAME) class CMD_CLASS_NAME;\
extern chaos::common::utility::ObjectFactoryInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectFactoryInstancer;\
class CMD_CLASS_NAME : public BASE_CLASS_NAME
			
#define DEFINE_CLASS_FACTORY(CMD_CLASS_NAME,BASE_CLASS_NAME) \
chaos::common::utility::ObjectFactoryInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectFactoryInstancer(new chaos::common::utility::ObjectFactoryAliasInstantiation<CMD_CLASS_NAME>(#CMD_CLASS_NAME));

#define DEFINE_CLASS_FACTORY_NO_ALIAS(CMD_CLASS_NAME,BASE_CLASS_NAME) \
chaos::common::utility::ObjectFactoryInstancer<BASE_CLASS_NAME> CMD_CLASS_NAME ## ObjectFactoryInstancer(new chaos::common::utility::ObjectFactoryInstantiation<CMD_CLASS_NAME>(#CMD_CLASS_NAME));

			
#define REGISTER_AND_DEFINE_DERIVED_CLASS_FACTORY_HELPER(t) \
friend class chaos::common::utility::ObjectFactoryAliasInstantiation<t>;

		}
	}
}
#endif

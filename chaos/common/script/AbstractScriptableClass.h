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

#ifndef __CHAOSFramework__AbstractScriptableClass_h
#define __CHAOSFramework__AbstractScriptableClass_h

#include <chaos/common/global.h>
#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/script/script_types.h>
#include <chaos/common/script/AbstractScriptVM.h>
namespace chaos {
    namespace common{
        namespace script {
            class ScriptManager;

                //! define the abstraction of a scriptable class
            /*!
             Evrey sublcass of this one can expose method that can be callable form
             a script language.
             */
            class AbstractScriptableClass {
                friend class ScriptManager;
                ScriptManager *script_manager_ptr;
                    //! is the name of the api class
                const std::string api_class_name;

                    //!entry point of the scirpting for call an exposed api
                virtual int callApi(const std::string& api_name,
                                    const ScriptInParam& input_parameter,
                                    ScriptOutParam& output_parameter) = 0;

            protected:
                //return the virtual machine to subclass
                AbstractScriptVM *getVM();

            public:
                AbstractScriptableClass(const std::string& _api_class_name);
                virtual ~AbstractScriptableClass();

                //!provide an initlization for the api environment
                virtual int init(const char * init_data = NULL){return 0;}

                //provide a way to deinitilize the evironneto initialized in the init call
                virtual void deinit(){}

                const std::string& getClassName(){return api_class_name;}
            };

                //!defin e atype specific api with utility for add and remove it
            template<typename T>
            class TemplatedAbstractScriptableClass:
            public AbstractScriptableClass {
            public:
                    //!define the scriptable API format
                typedef int (T::*ScriptableApi)(const ScriptInParam& input_parameter,
                                                ScriptOutParam& output_parameter);

            private:
                    //! defin ethe map for the api pointer associated with the name
                typedef std::map<std::string, ScriptableApi>                        MapNameApiPointer;
                typedef typename std::map<std::string, ScriptableApi>::iterator     MapNameApiPointerIterator;

                T* object_reference;

                    //! is the maph that correlate the name fo an api with his pointer.
                MapNameApiPointer map_api_ptr;

                    //!entry point of the scirpting for call an exposed api
                int callApi(const std::string& api_name,
                            const ScriptInParam& input_parameter,
                            ScriptOutParam& output_parameter) {
                        //check for api presence
                    if(map_api_ptr.count(api_name) == 0) {
                        ERR_LOG(TemplatedAbstractScriptableClass) << CHAOS_FORMAT("Api name %1% is not registered", %api_name);
                        return -1;
                    }
                        //we have the api, then we can call it
                     return ((*object_reference).*map_api_ptr[api_name])(input_parameter,
                                                             output_parameter);
                }

            protected:
                TemplatedAbstractScriptableClass(T* _object_reference,
                                                 const std::string& api_class_name):
                AbstractScriptableClass(api_class_name),
                object_reference(_object_reference){                }

                ~TemplatedAbstractScriptableClass(){
                    //clear all map entries
                    map_api_ptr.clear();
                }

                //! register a method as api
                void addApi(const std::string& api_name, ScriptableApi api_pointer) {
                    if(map_api_ptr.count(api_name) != 0) return;
                    map_api_ptr.insert(make_pair(api_name, api_pointer));
                }

                //! deregister an api
                void removeApi(const std::string& api_name) {
                    if(map_api_ptr.count(api_name) == 0) return;
                    map_api_ptr.erase(api_name);
                }
            };
        }
    }
}

#endif /* __CHAOSFramework__AbstractScriptableClass_h */

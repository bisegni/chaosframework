/*
 *	AbstractScriptableClass.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 10/05/16 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__AbstractScriptableClass_h
#define __CHAOSFramework__AbstractScriptableClass_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/data/CDataVariant.h>
#include <chaos/common/script/script_types.h>

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

                    //! is the name of the api class
                const std::string api_class_name;

                    //!entry point of the scirpting for call an exposed api
                virtual int callApi(const std::string& api_name,
                                    const ScriptInParam& input_parameter,
                                    ScriptOutParam& output_parameter) = 0;

            public:
                AbstractScriptableClass(const std::string& _api_class_name);
                virtual ~AbstractScriptableClass();
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

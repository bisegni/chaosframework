/*
 *	EUSearch.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 04/04/2017 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework_CA3C199C_419D_423D_A5A5_D3DF0EA14502_EUSearch_h
#define __CHAOSFramework_CA3C199C_419D_423D_A5A5_D3DF0EA14502_EUSearch_h

#include <chaos/common/script/AbstractScriptableClass.h>

#include <chaos/common/chaos_types.h>
#include <chaos/common/io/QueryCursor.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace script {
                //! forward declaration
                class ScriptableExecutionUnit;
                namespace api {
                    //! class taht permit to interact with execution unit and chaos cript framework
                    class EUSearch:
                    public chaos::common::script::TemplatedAbstractScriptableClass<EUSearch> {
                        friend class chaos::cu::control_manager::script::ScriptableExecutionUnit;
                        
                        //execution unit instances
                        ScriptableExecutionUnit *eu_instance;
                        
                        //define map of cursor and element
                        struct QueryInfo {
                            chaos::common::io::QueryCursor *cursor;
                            boost::shared_ptr<chaos::common::data::CDataWrapper> current_extracted_element;
                            QueryInfo();
                            QueryInfo(chaos::common::io::QueryCursor *_cursor);
                            QueryInfo(const QueryInfo& copy_src);
                            QueryInfo& operator=(QueryInfo const &rhs);
                        };
                        
                        CHAOS_DEFINE_MAP_FOR_TYPE(uint32_t, QueryInfo, MapCursor);
                        MapCursor map_cursor;
                        uint32_t cursor_count;
                        
                        uint32_t storeCursor(chaos::common::io::QueryCursor *new_cursor);
                        void removeCursor(uint32_t cursor_id);
                    public:
                        ~EUSearch();
                    protected:
                        EUSearch(ScriptableExecutionUnit *_eu_instance);
                        
                        //! start a new search
                        /*!
                         input parameter
                         node id[string] - the identification of the node
                         the domain [as KeyDataStorageDomain] - the domain stored by the node id
                         start timestamp [int64] - the start timestamp of the search
                         end timestamp [int64] - the end timestamp of the search
                         
                         output parameter
                         the id of the cursor[int32_t] to be used for other api
                         */
                        int newSearch(const common::script::ScriptInParam& input_parameter,
                                      common::script::ScriptOutParam& output_parameter);
                        
                        //! start a search from now since some time expressed in seconds
                        /*!
                         input parameter
                         node id[string] - the identification of the node
                         the domain [as KeyDataStorageDomain] - the domain stored by the node id
                         number of seconds since now[int32_t] - the number of seconds since now the
                         are idenitfied as start timestamp
                         
                         output parameter
                         the id of the cursor[int32_t] to be used for other api
                         */
                        int newSearchSinceSeconds(const common::script::ScriptInParam& input_parameter,
                                                  common::script::ScriptOutParam& output_parameter);
                        
                        //! close a cursor
                        /*!
                         input parameter
                         cursor id[int32] - the identification of the node
                         */
                        int closeSearch(const common::script::ScriptInParam& input_parameter,
                                        common::script::ScriptOutParam& output_parameter);
                        
                        //! check the cursor if contains other found record
                        /*!
                         input parameter
                         cursor id[int32] - the identification of the node
                         
                         output parameter
                         exist flag[boolean] - true if exists other element in cursor
                         */
                        int hasNext(const common::script::ScriptInParam& input_parameter,
                                    common::script::ScriptOutParam& output_parameter);
                        
                        //!exstract the next element e bring it in cache
                        /*!
                         input parameter
                         cursor id[int32] - the identification of the node
                         */
                        int next(const common::script::ScriptInParam& input_parameter,
                                 common::script::ScriptOutParam& output_parameter);
                        
                        //!exstract the next element e bring it in cache
                        /*!
                         input parameter
                         cursor id[int32] - the identification of the node
                         attribute name[string] - the name of the attribute of the current element
                         of the cursor
                         
                         output parameter
                         the value of the attribute
                         */
                        int getAttributeValue(const common::script::ScriptInParam& input_parameter,
                                              common::script::ScriptOutParam& output_parameter);
                    };
                }
            }
        }
    }
}

#endif /* __CHAOSFramework_CA3C199C_419D_423D_A5A5_D3DF0EA14502_EUSearch_h */

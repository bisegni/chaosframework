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


#include <chaos/cu_toolkit/control_manager/script/api/EUSearch.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

#include <chaos/common/utility/TimingUtil.h>

using namespace chaos::common::io;
using namespace chaos::common::data;
using namespace chaos::common::script;
using namespace chaos::common::utility;

using namespace chaos::cu::data_manager;
using namespace chaos::cu::control_manager::script;
using namespace chaos::cu::control_manager::script::api;

#define EUSW_LAPP    INFO_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())
#define EUSW_DBG     DBG_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())
#define EUSW_LERR    ERR_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())

#define ALIAS_NEW_SEARCH                "new"
#define ALIAS_NEW_OFFSET_SEARCH         "newWithOffset"
#define ALIAS_CLOSE_SEARCH              "close"
#define ALIAS_HAS_NEXT                  "hasNext"
#define ALIAS_NEXT                      "next"
#define ALIAS_GET_ATTRIBUTE_VALUE       "getAttributeValue"
#define ALIAS_ALGO_ON_OFFSET_SEARCH     "algoWithOffset"


#define DEFAULT_PAGE_LEN    100

#pragma mark QueryResult

EUSearch::QueryInfo::QueryInfo():
cursor(NULL){}

EUSearch::QueryInfo::QueryInfo(QueryCursor *_cursor):
cursor(_cursor){}

EUSearch::QueryInfo::QueryInfo(const EUSearch::QueryInfo& copy_src) {
    cursor = copy_src.cursor;
    current_extracted_element = copy_src.current_extracted_element;
}

EUSearch::QueryInfo& EUSearch::QueryInfo::operator=(EUSearch::QueryInfo const &rhs) {
    cursor = rhs.cursor;
    current_extracted_element = rhs.current_extracted_element;
    return *this;
}


#pragma mark EUSearch
EUSearch::EUSearch(ScriptableExecutionUnit *_eu_instance):
TemplatedAbstractScriptableClass(this,
                                 "search"),
eu_instance(_eu_instance),
cursor_count(0){
    //add all exposed api
    addApi(ALIAS_NEW_SEARCH, &EUSearch::newSearch);
    addApi(ALIAS_NEW_OFFSET_SEARCH, &EUSearch::newSearchSinceSeconds);
    addApi(ALIAS_CLOSE_SEARCH, &EUSearch::closeSearch);
    addApi(ALIAS_HAS_NEXT, &EUSearch::hasNext);
    addApi(ALIAS_NEXT, &EUSearch::next);
    addApi(ALIAS_GET_ATTRIBUTE_VALUE, &EUSearch::getAttributeValue);
    addApi(ALIAS_ALGO_ON_OFFSET_SEARCH, &EUSearch::runAlgoSearchSinceSeconds);
}

EUSearch::~EUSearch() {}

unsigned int EUSearch::storeCursor(QueryCursor *new_cursor) {
    map_cursor.insert(MapCursorPair(++cursor_count, new_cursor));
    return cursor_count;
}

void EUSearch::removeCursor(uint32_t cursor_id) {
    if(map_cursor.count(cursor_id) == 0) return;
    eu_instance->releseQuery(map_cursor[cursor_id].cursor);
    map_cursor.erase(cursor_id);
}

#pragma mark API
int EUSearch::newSearch(const common::script::ScriptInParam& input_parameter,
                        common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 4) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    try{
        uint32_t new_cursor_id = 0;
        QueryCursor *cursor = NULL;
        int err = eu_instance->performQuery(&cursor,
                                            input_parameter[0].asString(),//node_id
                                            static_cast<KeyDataStorageDomain>(input_parameter[1].asInt32()),
                                            input_parameter[2].asInt64(),//start_ts
                                            input_parameter[3].asInt64(),//end_ts;
                                            DEFAULT_PAGE_LEN);
        CHAOS_ASSERT(!err &&
                     cursor != NULL);
        if(!err &&
           cursor) {
            //we successfully have a cursor
            new_cursor_id = storeCursor(cursor);
        }
        //return the variant
        output_parameter.push_back(CDataVariant(new_cursor_id));
    } catch(...) {
        return -2;
    }
    return 0;
}

int EUSearch::newSearchSinceSeconds(const common::script::ScriptInParam& input_parameter,
                                    common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 3) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    try{
        uint32_t new_cursor_id = 0;
        QueryCursor *cursor = NULL;
        const uint64_t end_ts = TimingUtil::getTimeStamp();
        const uint64_t start_ts = end_ts - (input_parameter[2].asInt64()*1000);
        int err = eu_instance->performQuery(&cursor,
                                            input_parameter[0].asString(),//node_id
                                            static_cast<KeyDataStorageDomain>(input_parameter[1].asInt32()),
                                            start_ts,//start_ts
                                            end_ts,//end_ts;
                                            DEFAULT_PAGE_LEN);
        CHAOS_ASSERT(!err &&
                     cursor != NULL);
        if(!err &&
           cursor) {
            //we successfully have a cursor
            new_cursor_id = storeCursor(cursor);
        }
        //return the variant
        output_parameter.push_back(CDataVariant(new_cursor_id));
    } catch(...) {
        return -2;
    }
    return 0;
}

int EUSearch::closeSearch(const common::script::ScriptInParam& input_parameter,
                          common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 1) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    try{
        removeCursor(input_parameter[0].asInt32());
    } catch(...) {
        return -2;
    }
    return 0;
}

int EUSearch::hasNext(const common::script::ScriptInParam& input_parameter,
                      common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 1) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    try{
        uint32_t cursor_id = (uint32_t)input_parameter[0].asInt32();
        if(map_cursor.count(cursor_id) == 0) return -2;
        output_parameter.push_back(CDataVariant(map_cursor[cursor_id].cursor->hasNext()));
    } catch(...) {
        return -3;
    }
    return 0;
}

int EUSearch::next(const common::script::ScriptInParam& input_parameter,
                   common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 1) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    try{
        uint32_t cursor_id = (uint32_t)input_parameter[0].asInt32();
        if(map_cursor.count(cursor_id) == 0) return -2;
        if(map_cursor[cursor_id].cursor->hasNext()) {
            map_cursor[cursor_id].current_extracted_element = map_cursor[cursor_id].cursor->next();
        } else {
            return -3;
        }
    } catch(...) {
        return -4;
    }
    return 0;
}

int EUSearch::getAttributeValue(const common::script::ScriptInParam& input_parameter,
                                common::script::ScriptOutParam& output_parameter) {
    if(input_parameter.size() != 2) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    try{
        uint32_t cursor_id = (uint32_t)input_parameter[0].asInt32();
        const std::string atribute_name = input_parameter[1].asString();
        if(map_cursor.count(cursor_id) == 0) return -2;
        
        QueryInfo qi = map_cursor[cursor_id];
        if(qi.current_extracted_element.get() == NULL) return -3;
        
        if(qi.current_extracted_element->hasKey(atribute_name)) {
            output_parameter.push_back(qi.current_extracted_element->getVariantValue(atribute_name));
        } else {return -4;}
    } catch(...) {
        return -5;
    }
    return 0;
}

int EUSearch::runAlgoSearchSinceSeconds(const common::script::ScriptInParam& input_parameter,
                                        common::script::ScriptOutParam& output_parameter) {
    QueryCursor *cursor = NULL;
    uint64_t processed_lement = 0;
    ChaosStringVector attribute_name;
    AbstractScriptVM *vm_ptr = getVM();
    ChaosSharedPtr<CDataWrapper> found_element;
    common::script::ScriptInParam algo_input_parameter;
    
    CHAOS_ASSERT(vm_ptr);
    
    if(input_parameter.size() < 6) {
        EUSW_LERR << "Bad number of parameter";
        return -1;
    }
    
    const std::string node_id = input_parameter[0].asString();
    const KeyDataStorageDomain search_domain = static_cast<KeyDataStorageDomain>(input_parameter[1].asInt32());
    const uint64_t seconds_from_now = input_parameter[2].asInt64()*1000;
    const uint64_t end_ts = TimingUtil::getTimeStamp();
    const uint64_t start_ts = end_ts - seconds_from_now;
    
    const std::string algo_handler = input_parameter[3].asString();
    const std::string termination_handler = input_parameter[4].asString();
    
    //get wich attribute the algoritm need to use
    for(int idx = 5;
        idx < input_parameter.size();
        idx++) {
        attribute_name.push_back(input_parameter[idx].asString());
    }
    
    int err = eu_instance->performQuery(&cursor,
                                        node_id,
                                        search_domain,
                                        start_ts,
                                        end_ts,
                                        DEFAULT_PAGE_LEN);
    CHAOS_ASSERT(!err &&
                 cursor != NULL);
    if(err) return err;
    try {
        //we can proceed
        while(cursor->hasNext()) {
            processed_lement++;
            found_element = cursor->next();
            //fetch attribute as input parater
            algo_input_parameter.clear();
            for(ChaosStringVectorIterator it = attribute_name.begin(),
                end = attribute_name.end();
                it != end;
                it++) {
                algo_input_parameter.push_back(found_element->getVariantValue(*it));
            }
            
            //call algo
            if((err = vm_ptr->callProcedure(algo_handler,
                                            algo_input_parameter))) {
                EUSW_LERR << CHAOS_FORMAT("Error %1% calling algorithm handler %2%", %err%algo_handler);
                break;
            }
        }
    } catch (...) {}
    
    //release the query
    eu_instance->releseQuery(cursor);
    
    algo_input_parameter.clear();
    algo_input_parameter.push_back(CDataVariant(processed_lement));
    if((err = vm_ptr->callProcedure(termination_handler,
                                    algo_input_parameter))) {
        EUSW_LERR << CHAOS_FORMAT("Error %1% calling algorithm handler %2%", %err%algo_handler);
    }
    return 0;
}

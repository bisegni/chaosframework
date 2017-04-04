/*
 *	EUSearch.cpp
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


#include <chaos/cu_toolkit/control_manager/script/api/EUSearch.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::script;
using namespace chaos::cu::control_manager::script;
using namespace chaos::cu::control_manager::script::api;

#define EUSW_LAPP    INFO_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())
#define EUSW_DBG     DBG_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())
#define EUSW_LERR    ERR_LOG_1_P(EUDSValueManagement, eu_instance->getCUID())

EUSearch::EUSearch(ScriptableExecutionUnit *_eu_instance):
TemplatedAbstractScriptableClass(this,
                                 "eu"),
eu_instance(_eu_instance){
    //add all exposed api
//    addApi(EUSW_ADD_DATASET_ATTRIBUTE, &EUSearch::addDatasetAttribute);
//    addApi(EUSW_SET_OUTPUT_ATTRIBUTE_VALUE, &EUSearch::setOutputAttributeValue);
//    addApi(EUSW_GET_OUTPUT_ATTRIBUTE_VALUE, &EUSearch::getOutputAttributeValue);
//    addApi(EUSW_GET_INPUT_ATTRIBUTE_VALUE, &EUSearch::getInputAttributeValue);
}

EUSearch::~EUSearch() {}

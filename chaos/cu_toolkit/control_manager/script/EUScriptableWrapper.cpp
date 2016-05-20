/*
 *	EUScriptableWrapper.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by Claudio Bisegni.
 *
 *    	Copyright 13/05/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/control_manager/script/EUScriptableWrapper.h>
#include <chaos/cu_toolkit/control_manager/script/ScriptableExecutionUnit.h>

using namespace chaos::common::script;
using namespace chaos::cu::control_manager::script;

#define EUSW_LAPP    INFO_LOG(EUScriptableWrapper) << eu_instance->getCUID() << " - "
#define EUSW_DBG     DBG_LOG(EUScriptableWrapper) << eu_instance->getCUID() << " - "
#define EUSW_LERR    ERR_LOG(EUScriptableWrapper) << eu_instance->getCUID() << " - "


EUScriptableWrapper::EUScriptableWrapper(ScriptableExecutionUnit *_eu_instance):
TemplatedAbstractScriptableClass(this,
                                 "eu"),
eu_instance(_eu_instance){
    //add all exposed api
    addApi(EUSW_ADD_DATASET_ATTRIBUTE, &EUScriptableWrapper::addDatasetAttribute);
}

EUScriptableWrapper::~EUScriptableWrapper() {}

int EUScriptableWrapper::addDatasetAttribute(const ScriptInParam& input_parameter,
                                             ScriptOutParam& output_parameter) {
    EUSW_LAPP<<"Call of addDatasetAttribute";
    return 0;
}
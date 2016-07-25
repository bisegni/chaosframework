/*
 *	Lists.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 25/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/data/structured/Lists.h>

using namespace chaos::common::data::structured;

#pragma mark ChaosStringVectorSDWrapper
ChaosStringVectorSDWrapper::ChaosStringVectorSDWrapper():
ChaosStringVectorSDWrapperSubclass(){}

ChaosStringVectorSDWrapper::ChaosStringVectorSDWrapper(const ChaosStringVector& copy_source):
ChaosStringVectorSDWrapperSubclass(copy_source){}

ChaosStringVectorSDWrapper::ChaosStringVectorSDWrapper(chaos::common::data::CDataWrapper *serialized_data):
ChaosStringVectorSDWrapperSubclass(serialized_data){deserialize(serialized_data);}

void ChaosStringVectorSDWrapper::deserialize(chaos::common::data::CDataWrapper *serialized_data) {
    if(serialized_data == NULL) return;
    ChaosStringVectorSDWrapperSubclass::dataWrapped().clear();
    const std::string ser_key = "std_vector_"+serialization_postfix;
    if(serialized_data->hasKey(ser_key) &&
       serialized_data->isVectorValue(ser_key)) {
        std::auto_ptr<chaos::common::data::CMultiTypeDataArrayWrapper> serialized_array(serialized_data->getVectorValue(ser_key));
        for(int idx = 0;
            idx < serialized_array->size();
            idx++) {
            const std::string element(serialized_array->getStringElementAtIndex(idx));
            ChaosStringVectorSDWrapperSubclass::dataWrapped().push_back(element);
        }
    }
}

std::auto_ptr<chaos::common::data::CDataWrapper> ChaosStringVectorSDWrapper::serialize(const uint64_t sequence) {
    std::auto_ptr<chaos::common::data::CDataWrapper> result(new chaos::common::data::CDataWrapper());
    const std::string ser_key = "std_vector_"+serialization_postfix;
    for(ChaosStringVectorIterator it = ChaosStringVectorSDWrapperSubclass::dataWrapped().begin(),
        end = ChaosStringVectorSDWrapperSubclass::dataWrapped().end();
        it != end;
        it++) {
        result->appendStringToArray(*it);
    }
    result->finalizeArrayForKey(ser_key);
    return result;
}

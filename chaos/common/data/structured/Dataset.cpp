/*
 *	Dataset.cpp
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

#include <chaos/common/data/structured/Dataset.h>

using namespace chaos::common::data::structured;

Dataset::Dataset():
name(),
post_fix(),
type(chaos::DataType::DatasetTypeUndefined){
    
}

Dataset::Dataset(const Dataset& copy_src):
name(copy_src.name),
post_fix(copy_src.post_fix),
type(copy_src.type){}

Dataset& Dataset::operator=(Dataset const &rhs) {
    name = rhs.name;
    post_fix = rhs.post_fix;
    type = rhs.type;
    attribute_list = rhs.attribute_list;
    return *this;
}

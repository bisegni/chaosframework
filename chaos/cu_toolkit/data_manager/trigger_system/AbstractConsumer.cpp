/*
 *	AbstractConsumer.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 28/07/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/trigger_system/AbstractConsumer.h>

#include <chaos/common/utility/UUIDUtil.h>

using namespace chaos::common::utility;
using namespace chaos::common::data::structured;
using namespace chaos::cu::data_manager::trigger_system;
#pragma mark AbstractConsumer
AbstractConsumer::AbstractConsumer(const std::string& consumer_name):
consumer_uuid(UUIDUtil::generateUUIDLite()),
configuration_dataset_element(new DatasetElement(0,
                                                 DatasetPtr(new Dataset(consumer_name,
                                                                        chaos::DataType::DatasetTypeInput)))){}

AbstractConsumer::~AbstractConsumer() {}

const std::string& AbstractConsumer::getConsumerUUID() const {
    return consumer_uuid;
}

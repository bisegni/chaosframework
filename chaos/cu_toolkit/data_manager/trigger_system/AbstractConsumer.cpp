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
using namespace chaos::common::property;
using namespace chaos::cu::data_manager::trigger_system;
#pragma mark AbstractConsumer
AbstractConsumer::AbstractConsumer(const std::string& _consumer_name,
                                   const std::string& _consumer_description):
PropertyGroup(_consumer_name),
consumer_name(_consumer_name),
consumer_description(_consumer_description),
consumer_uuid(UUIDUtil::generateUUIDLite()){}

AbstractConsumer::~AbstractConsumer() {}

const std::string& AbstractConsumer::getConsumerUUID() const {
    return consumer_uuid;
}

const std::string& AbstractConsumer::getConsumerName() const {
    return consumer_name;
}

const std::string& AbstractConsumer::getConsumerDescription() const {
    return consumer_description;
}

void AbstractConsumer::updateProperty(const PropertyGroup& property_group) {
    //we need to reaset all values
    for (MapPropertiesConstIterator it = property_group.getAllProperties().begin(),
         end = property_group.getAllProperties().end();
         it != end;
         it++) {
        PropertyGroup::setPropertyValue(it->first, it->second.getPropertyValue());
    }
}

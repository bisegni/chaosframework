/*
 *	RegisterEnviroment.cpp
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 01/08/16 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/data_manager/trigger_system/dataset_event/RegisterEnviroment.h>

using namespace chaos::cu::data_manager::trigger_system::dataset_event;

DatasetRegisterEvenironment::DatasetRegisterEvenironment():
RegisterEventConsumerEnvironment<DatasetEvent,
DatasetConsumer,
DatasetSubject,
ETDatasetAttributeType>("DatasetAttribute") {
    //registerConsumerClass<<#typename Consumer#>>(<#const std::string &consumer_name#>)
}

DatasetRegisterEvenironment::~DatasetRegisterEvenironment() {
    
}

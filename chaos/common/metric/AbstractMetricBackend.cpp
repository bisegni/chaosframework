/*
 *	AbstractMetricBackend.cpp
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2015 INFN, National Institute of Nuclear Physics
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

#include <chaos/common/metric/AbstractMetricBackend.h>

using namespace chaos::common::metric;

AbstractMetricBackend::AbstractMetricBackend(const std::string& _backend_indetity):
backend_indetity(_backend_indetity){
    
}

AbstractMetricBackend::~AbstractMetricBackend() {
    
}

void AbstractMetricBackend::init(void *init_data) throw(chaos::CException) {
    
}

void AbstractMetricBackend::deinit() throw(chaos::CException) {
    
}

void AbstractMetricBackend::addConfiguration(const std::string& configuraiton_name,
                                             const std::string& configuraiton_value) {
    //append configuration
    map_configuration.insert(make_pair(configuraiton_name, configuraiton_value));
}

void AbstractMetricBackend::addConfiguration(const MapMetricBackendConfiguration& configuration) {
    //apoend configuration for input map
    map_configuration.insert(configuration.begin(), configuration.end());
}
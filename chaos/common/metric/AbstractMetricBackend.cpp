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

#include <chaos/common/metric/AbstractMetricBackend.h>

using namespace chaos::common::metric;

AbstractMetricBackend::AbstractMetricBackend(const std::string& _backend_indetity):
backend_indetity(_backend_indetity){
    
}

AbstractMetricBackend::~AbstractMetricBackend() {
    
}

void AbstractMetricBackend::init(void *init_data)  {
    
}

void AbstractMetricBackend::deinit()  {
    
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

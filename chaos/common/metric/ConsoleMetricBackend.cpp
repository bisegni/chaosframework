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

#include <chaos/common/metric/ConsoleMetricBackend.h>

using namespace chaos::common::metric;

#define CONSOLE_MB_INFO INFO_LOG(ConsoleMetricBackend)
#define CONSOLE_MB_LDBG DBG_LOG(ConsoleMetricBackend)
#define CONSOLE_MB_LNOTICE NOTICE_LOG(ConsoleMetricBackend)

ConsoleMetricBackend::ConsoleMetricBackend(const std::string& _backend_identity):
AbstractMetricBackend(_backend_identity){}

ConsoleMetricBackend::~ConsoleMetricBackend() {}

void ConsoleMetricBackend::preMetric() {
    if(first_metric) return;
    output_stream << ",";
}

void ConsoleMetricBackend::addMetric(const std::string& metric_name,
                                     const std::string& metric_value) {
    //!
    output_stream << metric_name << ":" << metric_value;
    first_metric = false;
}


void ConsoleMetricBackend::postMetric() {
    
}

void ConsoleMetricBackend::prepare(uint64_t metric_acquire_ts) {
    first_metric = true;
    output_stream << backend_indetity << "[";
}

void ConsoleMetricBackend::flush() {
    output_stream <<"]";
    CONSOLE_MB_LNOTICE << output_stream.str();
    output_stream.clear();
    output_stream.str("");
}
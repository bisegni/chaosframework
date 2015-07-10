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
#define CONSOLE_MB_LERR ERR_LOG(ConsoleMetricBackend)

ConsoleMetricBackend::ConsoleMetricBackend(const std::string& _backend_identity):
AbstractMetricBackend(_backend_identity){}

ConsoleMetricBackend::~ConsoleMetricBackend() {}

void ConsoleMetricBackend::addMetric(const std::string& metric_name,
                                     const std::string& metric_value) {
    //!
    output_stream << metric_name << ":" << metric_value << ",";
}

void ConsoleMetricBackend::prepare() {
    output_stream << backend_indetity << "[";
}

void ConsoleMetricBackend::flush() {
    std::string output_str = output_stream.str();
    output_stream.clear();
    if(output_str.find_last_of(",") != string::npos) {
        output_str.resize(output_str.size()-1);
    }
    output_str.append("]");
    CONSOLE_MB_INFO << output_str;
}
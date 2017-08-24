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

#include <chaos/common/global.h>
#include <chaos/common/metric/ConsoleMetricBackend.h>

using namespace chaos::common::metric;

#define CONSOLE_MB_INFO INFO_LOG(ConsoleMetricBackend)
#define CONSOLE_MB_LDBG DBG_LOG(ConsoleMetricBackend)
#define CONSOLE_MB_LNOTICE NOTICE_LOG(ConsoleMetricBackend)

ConsoleMetricBackend::ConsoleMetricBackend(const std::string& _backend_identity):
AbstractMetricBackend(_backend_identity),
first_metric(false){}

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

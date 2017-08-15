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

#ifndef __CHAOSFramework__ConsoleMetricBackend__
#define __CHAOSFramework__ConsoleMetricBackend__

#include <chaos/common/metric/AbstractMetricBackend.h>

#include <sstream>

namespace chaos {
    namespace common {
        namespace metric {
            
            class ConsoleMetricBackend:
            public AbstractMetricBackend {
                bool first_metric;
                std::ostringstream output_stream;
            public:
                ConsoleMetricBackend(const std::string& _backend_identity);
                ~ConsoleMetricBackend();
                void preMetric();
                void addMetric(const std::string& metric_name,
                                       const std::string& metric_value);
                void postMetric();
                void prepare(uint64_t metric_acquire_ts);
                void flush();
            };
            
        }
    }
}

#endif /* defined(__CHAOSFramework__ConsoleMetricBackend__) */

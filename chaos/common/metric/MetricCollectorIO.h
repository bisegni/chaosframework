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

#ifndef __CHAOSFramework__MetricCollectorIO__
#define __CHAOSFramework__MetricCollectorIO__

#include <chaos/common/metric/MetricCollector.h>

#include <chaos/common/async_central/async_central.h>

#include <boost/atomic.hpp>
namespace chaos {
    namespace common {
        namespace metric {
            
            //! class for data io metric collector
            class MetricCollectorIO:
            public MetricCollector,
            public chaos::common::async_central::TimerHandler {
            protected:
                uint64_t last_sample_ts;
                void timeout();
                virtual void fetchMetricForTimeDiff(uint64_t time_diff) = 0;
            public:
                MetricCollectorIO();
                ~MetricCollectorIO();
                void startLogging();
                void stopLogging();
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__MetricCollectorIO__) */

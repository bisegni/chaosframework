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

#ifndef __CHAOSFramework__DeviceSharedDataWorkerMetricCollector__
#define __CHAOSFramework__DeviceSharedDataWorkerMetricCollector__

#include "DeviceSharedDataWorker.h"
#include <chaos/common/metric/metric.h>

#include <memory>

#include <boost/thread.hpp>

namespace chaos{
    namespace metadata_service {
        namespace worker {
            
            class DeviceSharedDataWorkerMetricCollector:
            public DeviceSharedDataWorker {
                chaos::common::metric::GaugeUniquePtr   gauge_queued_dataset_uptr;
                chaos::common::metric::GaugeUniquePtr   gauge_queued_memory_uptr;
                chaos::common::metric::CounterUniquePtr counter_rejeted_element_uptr;
            protected:
                void executeJob(WorkerJobPtr job_info, void* cookie);
            public:
                DeviceSharedDataWorkerMetricCollector();
                ~DeviceSharedDataWorkerMetricCollector();
                int submitJobInfo(WorkerJobPtr job_info, int64_t milliseconds_to_wait = chaos::common::constants::MDSHistoryQueuePushTimeoutinMSec);
                
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceSharedDataWorkerMetricCollector__) */

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
#include "DeviceSharedDataWorkerMetric.h"

#include <memory>

#include <boost/thread.hpp>

namespace chaos{
    namespace metadata_service {
        namespace worker {
            
            class DeviceSharedDataWorkerMetricCollector:
            public DeviceSharedDataWorker {
                ChaosSharedPtr<DeviceSharedDataWorkerMetric> data_worker_metric;
            protected:
                void executeJob(WorkerJobPtr job_info, void* cookie);
            public:
                DeviceSharedDataWorkerMetricCollector(ChaosSharedPtr<DeviceSharedDataWorkerMetric> _data_worker_metric);
                ~DeviceSharedDataWorkerMetricCollector();
                int submitJobInfo(WorkerJobPtr job_info);
                
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceSharedDataWorkerMetricCollector__) */

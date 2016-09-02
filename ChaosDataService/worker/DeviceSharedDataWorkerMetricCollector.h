/*
 *	DeviceSharedDataWorkerMetricCollector.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2014 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__DeviceSharedDataWorkerMetricCollector__
#define __CHAOSFramework__DeviceSharedDataWorkerMetricCollector__

#include "DeviceSharedDataWorker.h"
#include "DeviceSharedDataWorkerMetric.h"

#include <memory>

#include <boost/thread.hpp>

namespace chaos{
    namespace data_service {
        namespace worker {
            
            class DeviceSharedDataWorkerMetricCollector:
            public DeviceSharedDataWorker {
                boost::shared_ptr<DeviceSharedDataWorkerMetric> data_worker_metric;
            protected:
                void executeJob(WorkerJobPtr job_info, void* cookie);
            public:
                DeviceSharedDataWorkerMetricCollector(const std::string& _cache_impl_name,
                                                      const std::string& _db_impl_name,
                                                      boost::shared_ptr<DeviceSharedDataWorkerMetric> _data_worker_metric);
                ~DeviceSharedDataWorkerMetricCollector();
                int submitJobInfo(WorkerJobPtr job_info);
                
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__DeviceSharedDataWorkerMetricCollector__) */

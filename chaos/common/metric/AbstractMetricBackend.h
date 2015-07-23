/*
 *	AbstractMetricBackend.h
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

#ifndef __CHAOSFramework__AbstractMetricBackend__
#define __CHAOSFramework__AbstractMetricBackend__

#include <string>
#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/InizializableService.h>

#include <boost/variant.hpp>

#include <stdint.h>

namespace chaos {
    namespace common {
        namespace metric {
            
            CHAOS_DEFINE_MAP_FOR_TYPE(std::string, std::string, MapMetricBackendConfiguration)
            
            //!Abstract class for the metric representation and memorization
            /*!
             To create a new backnd for visualization or memorization on various backend
             */
            class AbstractMetricBackend:
            public chaos::common::utility::InizializableService {
            protected:
                const std::string backend_indetity;
                MapMetricBackendConfiguration map_configuration;
            public:
                AbstractMetricBackend(const std::string& _backend_indetity);
                ~AbstractMetricBackend();
                
                void init(void *init_data) throw(chaos::CException);
                void deinit() throw(chaos::CException);
                void addConfiguration(const std::string& configuraiton_name,
                                      const std::string& configuraiton_value);
                void addConfiguration(const MapMetricBackendConfiguration& configuration);
                virtual void preMetric() = 0;
                virtual void addMetric(const std::string& metric_name,
                                       const std::string& metric_value) = 0;
                virtual void postMetric() = 0;
                virtual void prepare() = 0;
                virtual void flush() = 0;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractMetricBackend__) */

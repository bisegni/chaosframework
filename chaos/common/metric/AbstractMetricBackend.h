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
                
                void init(void *init_data) ;
                void deinit() ;
                void addConfiguration(const std::string& configuraiton_name,
                                      const std::string& configuraiton_value);
                void addConfiguration(const MapMetricBackendConfiguration& configuration);
                virtual void preMetric() = 0;
                virtual void addMetric(const std::string& metric_name,
                                       const std::string& metric_value) = 0;
                virtual void postMetric() = 0;
                virtual void prepare(uint64_t metric_acquire_ts) = 0;
                virtual void flush() = 0;
            };
        }
    }
}

#endif /* defined(__CHAOSFramework__AbstractMetricBackend__) */

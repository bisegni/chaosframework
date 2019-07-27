/*
 * Copyright 2012, 2019 INFN
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


#ifndef MetricManager_h
#define MetricManager_h

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/Singleton.h>
#include <chaos/common/utility/InizializableService.h>

#include <prometheus/exposer.h>
#include <prometheus/registry.h>



namespace chaos {
    namespace common {
        namespace metric {
            
            /*!
             Central managment for metric exposition to Prometheus
             */
            class MetricManager:
            public common::utility::InizializableService,
            public chaos::common::utility::Singleton<MetricManager> {
                friend class chaos::common::utility::Singleton<MetricManager>;
                
                ChaosUniquePtr<prometheus::Exposer> http_exposer;
                
                MetricManager();
                ~MetricManager();
            protected:
                void init(void *data);
                void deinit();
            };
        }
    }
}

#endif /* MetricManager_hpp */

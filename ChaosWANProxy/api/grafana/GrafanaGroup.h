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
#ifndef __CHAOSFramework__GrafanaGroup__
#define __CHAOSFramework__GrafanaGroup__

#include "../AbstractApiGroup.h"

namespace chaos {
	namespace wan_proxy {
		namespace api {
            namespace grafana {
				
				//! group for all producer api
				class GrafanaGroup:
				public AbstractApiGroup {
				public:
					GrafanaGroup(persistence::AbstractPersistenceDriver *_persistence_driver);
					~GrafanaGroup();
				};
				
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__GrafanaGroup__) */

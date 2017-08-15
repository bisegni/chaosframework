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

#ifndef __CHAOSFramework__PublishDataService__
#define __CHAOSFramework__PublishDataService__

#include <string>

namespace chaos {
	namespace common {
		namespace data {
			namespace broker {
				namespace publisher {
					
					//! Publisher Service
					/*!
					 This class represent
					 */
					class PublisherDataService {
						
						std::string publisServiceName;
						
						PublisherDataService(const char *keyName);
						PublisherDataService(std::string& keyName);
						~PublisherDataService();
					public:
					};
				}
			}
		}
    }
}

#endif /* defined(__CHAOSFramework__PublisherDataService__) */

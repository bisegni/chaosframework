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

#ifndef __CHAOSFramework__ReaderDataService__
#define __CHAOSFramework__ReaderDataService__

#include <string>

namespace chaos {
	namespace common {
		namespace data {
			namespace broker {
				
				class DataBroker;
				
				namespace reader {
					
					class ReaderDataService {
						
						friend class chaos::common::data::broker::DataBroker;
						
						std::string publisServiceName;
						
						ReaderDataService(const char *keyName);
						ReaderDataService(std::string& keyName);
						~ReaderDataService();
					};
				}
			}
		}
	}
}

#endif /* defined(__CHAOSFramework__ReaderDataService__) */

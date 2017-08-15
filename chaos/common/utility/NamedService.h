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
#ifndef __CHAOSFramework__NamedService__
#define __CHAOSFramework__NamedService__

#include <string>
namespace chaos {
	namespace common {
		namespace utility {

			
			//! Named service utilityes class
			/*!
			 This class is a base class for all service that need to be recognized with an alias
			 */
			class NamedService {
				const std::string service_name;
			protected:
				explicit NamedService(const std::string & _service_name);
			public:
				/*
				 Return the alias of the current instance
				 \return the alias
				 */
				const std::string & getName() const;
			};
		}
	}
}
#endif /* defined(__CHAOSFramework__NamedService__) */

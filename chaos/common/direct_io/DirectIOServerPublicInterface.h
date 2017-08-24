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
#ifndef CHAOSFramework_AbstractDirectIOServer_h
#define CHAOSFramework_AbstractDirectIOServer_h

#include <stdint.h>
#include <chaos/common/utility/UrlAccessibleService.h>
namespace chaos {
	namespace common {
		namespace direct_io {
            
            /*!
             Direct io server public interface
             */
			class DirectIOServerPublicInterface:
			public chaos::common::utility::UrlAccessibleService {
            public:
                virtual uint32_t getPriorityPort() = 0;
                virtual uint32_t getServicePort() = 0;
				const std::string& getUrl() = 0;
            };
            
        }
    }
}

#endif

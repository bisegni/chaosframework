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
#ifndef CHAOSFramework_DirectIOEndpointHandler_h
#define CHAOSFramework_DirectIOEndpointHandler_h

#include <chaos/common/direct_io/DirectIOHandler.h>
#include <chaos/common/direct_io/DirectIODataPack.h>
namespace chaos {
	namespace common {
		namespace direct_io {
			class DirectIOEndpointHandler {
			public:
                virtual int consumeDataPack(chaos::common::direct_io::DirectIODataPackUPtr data_pack,
                                            chaos::common::direct_io::DirectIODataPackSPtr& synchronous_answer) = 0;
			};
		}
	}
}

#endif

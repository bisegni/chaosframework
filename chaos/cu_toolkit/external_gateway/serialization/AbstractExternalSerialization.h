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

#ifndef __CHAOSFramework__36DA8C2_0830_470B_9B3F_094BDE63046E_AbstractSerialization_h
#define __CHAOSFramework__36DA8C2_0830_470B_9B3F_094BDE63046E_AbstractSerialization_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/data/CDataBuffer.h>
namespace chaos{
    namespace cu {
        namespace external_gateway {
            namespace serialization {
            
                class AbstractExternalSerialization {
                    
                public:
                    AbstractExternalSerialization();
                    virtual ~AbstractExternalSerialization();
                    
                    virtual ChaosUniquePtr<chaos::common::data::CDataBuffer> serialize(const chaos::common::data::CDataWrapper& cdw_in) = 0;
                    virtual ChaosUniquePtr<chaos::common::data::CDataWrapper> deserialize(const chaos::common::data::CDataBuffer& cdb_in) = 0;
                };
            }
        }
    }
}

#endif /* __CHAOSFramework__36DA8C2_0830_470B_9B3F_094BDE63046E_AbstractSerialization_h */

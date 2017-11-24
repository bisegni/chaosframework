/*
 * Copyright 2012, 21/11/2017 INFN
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

#ifndef __CHAOSFramework__5D1E9AF_B79A_4FD7_EDRF_3CD7D8E7925B_ExternalBSONJsonSerialization_h
#define __CHAOSFramework__5D1E9AF_B79A_4FD7_EDRF_3CD7D8E7925B_ExternalBSONJsonSerialization_h
#include <chaos/common/external_unit/serialization/AbstractExternalSerialization.h>
namespace chaos{
    namespace common {
        namespace external_unit {
            namespace serialization {
                
                //!serialization class that convert CDataWrapper to/from Bson serialization
                class ExternalBSONJsonSerialization:
                public AbstractExternalSerialization {
                    
                public:
                    //serialization type
                    static const std::string ex_serialization_type;
                    
                    ExternalBSONJsonSerialization();
                    virtual ~ExternalBSONJsonSerialization();
                    
                    virtual ChaosUniquePtr<chaos::common::data::CDataBuffer> serialize(const chaos::common::data::CDataWrapper& cdw_in);
                    virtual ChaosUniquePtr<chaos::common::data::CDataWrapper> deserialize(const chaos::common::data::CDataBuffer& cdb_in);
                };
                
            }
        }
    }
}
#endif /* __CHAOSFramework__5D1E9AF_B79A_4FD7_EDRF_3CD7D8E7925B_ExternalBSONJsonSerialization_hh */

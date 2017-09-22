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

#ifndef RecoverError_h
#define RecoverError_h

#include "../mds_service_batch.h"

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;
            namespace control_unit {
                //!Batch command for recover error on one or more control unit
                class RecoverError:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS
                    uint32_t retry_number;
                    
                    //array that contain all requests
                    std::vector< ChaosSharedPtr<RequestInfo> > requests;
                    
                    ChaosUniquePtr<chaos::common::data::CMultiTypeDataArrayWrapper> cu_ids;
                public:
                    RecoverError();
                    ~RecoverError();
                protected:
                    // inherited method
                    void setHandler(chaos_data::CDataWrapper *data);
                    
                    // inherited method
                    void acquireHandler();
                    
                    // inherited method
                    void ccHandler();
                    
                    // inherited method
                    bool timeoutHandler();
                };
            }
        }
    }
}

#endif /* RecoverError_hpp */

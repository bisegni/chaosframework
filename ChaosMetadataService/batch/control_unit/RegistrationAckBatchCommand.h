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
#ifndef __CHAOSFramework__RegistrationAckBatchCommand__
#define __CHAOSFramework__RegistrationAckBatchCommand__

#include "../mds_service_batch.h"

namespace chaos {
    namespace metadata_service{
        namespace batch {
            class MDSBatchExcecutor;

            namespace control_unit {

                    //!Batch commadn for send ack to the control unit
                class RegistrationAckBatchCommand:
                public metadata_service::batch::MDSBatchCommand {
                    DECLARE_MDS_COMMAND_ALIAS

                    chaos::common::network::CNetworkAddress * control_unit_address;
                    ChaosUniquePtr<RequestInfo> request;

                    std::string cu_id;
                    std::string unit_server_addr;
                    int32_t     reg_result;
                public:
                    RegistrationAckBatchCommand();
                    ~RegistrationAckBatchCommand();
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

#endif /* defined(__CHAOSFramework__RegistrationAckBatchCommand__) */

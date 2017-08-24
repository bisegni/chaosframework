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

#ifndef __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h
#define __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h

#include <chaos/cu_toolkit/driver_manager/driver/AbstractRemoteIODriver.h>

namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                //!Opcode Ext driver message mapper
                /*!
                 this is the base class that permit to map the opcode sent by Controlunit
                 to the external driver command
                 */
                class OpcodeExternalCommandMapper {
                    AbstractRemoteIODriver *remote_driver;
                protected:
                    //!Send raw request to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                       chaos::common::data::CDWShrdPtr& message_response,
                                       uint32_t timeout = 5000);
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    int sendRawMessage(chaos::common::data::CDWUniquePtr message_data);
                    
                    const int getNumberOfMaxConnection() const;
                    void setNumberOfMaxConnection(int max_conenction);
                public:
                    OpcodeExternalCommandMapper(AbstractRemoteIODriver *_remote_driver);
                    virtual ~OpcodeExternalCommandMapper();
                    
                    virtual MsgManagmentResultType::MsgManagmentResult execOpcode(DrvMsgPtr cmd) = 0;
                    virtual int asyncMessageReceived(chaos::common::data::CDWUniquePtr message) = 0;
                };
                
            }
        }
    }
}

#endif /* __CHAOSFramework__B51A442_6610_45C9_9855_E6E15E2F3DA2_OpcodeExternalCommandMapper_h */

//
//  RemoteIODriverProtocol.h
//  CHAOSFramework
//
//  Created by bisegni on 12/10/2017.
//  Copyright © 2017 INFN. All rights reserved.
//

#ifndef RemoteIODriverProtocol_h
#define RemoteIODriverProtocol_h
namespace chaos {
    namespace cu {
        namespace driver_manager {
            namespace driver {
                
                class RemoteIODriverProtocol {
                public:
                    //!Send raw request to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     \param received_data si the raw data received from the driver
                     */
                    virtual int sendRawRequest(chaos::common::data::CDWUniquePtr message_data,
                                               chaos::common::data::CDWShrdPtr& message_response,
                                               uint32_t timeout = 5000) = 0;
                    
                    //!Send an opcode standard request to remote layer
                    /*!
                     \param opcode is the opcode of the message
                     \param opcode_data is the parameter associated to the opcode
                     \param received_data si the raw data received from the driver
                     */
                    virtual int sendOpcodeRequest(const std::string opcode,
                                                  chaos::common::data::CDWUniquePtr opcode_parameter,
                                                  chaos::common::data::CDWShrdPtr& message_response) = 0;
                    
                    //!Send raw message to the remote driver
                    /*!
                     \param message_data is the raw data to be transmitted to the remote driver
                     */
                    virtual int sendRawMessage(chaos::common::data::CDWUniquePtr message_data) = 0;
                    
                    //!!Send an opcode standard message to remote layer
                    /*!
                     \param opcode is the opcode of the message
                     \param opcode_data is the parameter associated to the opcode
                     */
                    virtual int sendOpcodeMessage(const std::string opcode,
                                                  chaos::common::data::CDWUniquePtr opcode_message) = 0;
                };
            }
        }
    }
}

#endif /* RemoteIODriverProtocol_h */

//
//  BatchCommandSandboxEventHandler.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/10/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_BatchCommandSandboxEventHandler_h
#define CHAOSFramework_BatchCommandSandboxEventHandler_h

#include <chaos/common/data/CDataWrapper.h>
#include <chaos/common/batch_command/BatchCommandTypes.h>

namespace chaos{
    namespace common {
        namespace batch_command {
            
            //forward declaration
            class BatchCommandSandbox;
            
            
            struct BatchCommandStat {
                uint32_t queued_commands;
                uint32_t stacked_commands;
                BatchCommandStat():
                queued_commands(0),
                stacked_commands(0){}
            };
            
            //! Handler interface for slow command sandbox event
            class BatchCommandSandboxEventHandler {
                
                // let sand box be friend of this class to use interface method
                friend class BatchCommandSandbox;
                
            protected:
                //! general command event handler
                /*!
                 \param command_seq sequence code that identify a command
                 \param type the type of the event that occour to the command
                 \param type_attribute_ptr the attribute of the command that is specified to the type
                 */
                virtual void handleCommandEvent(const std::string& command_alias,
                                                uint64_t command_seq,
                                                BatchCommandEventType::BatchCommandEventType type,
                                                chaos::common::data::CDataWrapper *command_info,
                                                const BatchCommandStat& commands_stats) = 0;
				
				//! general sandbox event handler
				/*!
				 \param sandbox_idx the idx of the sandobx that is firing the event
				 \param type the type of the event that occour to the sandbox
				 \param type_value_ptr the value of the command event that is specified to the type
				 \param type_value_size the size of value of the event
				 */
				virtual void handleSandboxEvent(const std::string& sandbox_id,
                                                BatchSandboxEventType::BatchSandboxEventType type,
                                                void* type_value_ptr = NULL,
                                                uint32_t type_value_size = 0) = 0;
            };
        }
    }
}

#endif

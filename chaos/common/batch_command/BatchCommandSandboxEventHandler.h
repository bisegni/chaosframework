//
//  BatchCommandSandboxEventHandler.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/10/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_BatchCommandSandboxEventHandler_h
#define CHAOSFramework_BatchCommandSandboxEventHandler_h

#include <chaos/common/batch_command/BatchCommandTypes.h>

namespace chaos{
    namespace common {
        namespace batch_command {
            
            //forward declaration
            class BatchCommandSandbox;
            
            
            //! Handler interface for slow command sandbox event
            class BatchCommandSandboxEventHandler {
                
                // let sand box be friend of this class to use interface method
                friend class BatchCommandSandbox;
                
            protected:
                //! general event handler
                /*!
                 \param command_seq sequence code that identify a command
                 \param type the type of the event that occour to the command
                 \param type_attribute_ptr the attribute of the command that is specified to the type
                 */
                virtual void handleEvent(uint64_t command_seq, BatchCommandEventType::BatchCommandEventType type, void* type_attribute_ptr) = 0;
            };
            
        }
    }
}

#endif

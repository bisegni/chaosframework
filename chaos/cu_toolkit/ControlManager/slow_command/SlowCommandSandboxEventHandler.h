//
//  SlowCommandSandboxEventHandler.h
//  CHAOSFramework
//
//  Created by Claudio Bisegni on 22/10/13.
//  Copyright (c) 2013 INFN. All rights reserved.
//

#ifndef CHAOSFramework_SlowCommandSandboxEventHandler_h
#define CHAOSFramework_SlowCommandSandboxEventHandler_h

#include <chaos/cu_toolkit/ControlManager/slow_command/SlowCommandTypes.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace slow_command {
				
				//forward declaration
				class SlowCommandSandbox;
				
				
				//! Handler interface for slow command sandbox event
				class SlowCommandSandboxEventHandler {
					
					// let sand box be friend of this class to use interface method
					friend class SlowCommandSandbox;
					
				protected:
					//! general event handler
					/*!
						\param command_seq sequence code that identify a command
						\param type the type of the event that occour to the command
						\param type_attribute_ptr the attribute of the command that is specified to the type
					 */
					virtual void handleEvent(uint64_t command_seq, SlowCommandEventType::SlowCommandEventType type, void* type_attribute_ptr) = 0;
				};
				
			}
		}
	}
}

#endif

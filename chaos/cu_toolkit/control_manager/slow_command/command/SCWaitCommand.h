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
#ifndef SCWaitCommand_h
#define SCWaitCommand_h


#include <chaos/cu_toolkit/control_manager/slow_command/SlowCommand.h>

namespace chaos {
    namespace cu {
        namespace control_manager {
            namespace slow_command {
                
                namespace command {
                    
                    //! Set Property Command
                    /*!
                     This command permit to update an attribute into the shared setting instance
                     of the sandbox.
                     */
                    DEFINE_BATCH_COMMAND_CLASS(SCWaitCommand, SlowCommand) {
                    protected:
                        // return the implemented handler
                        uint8_t implementedHandler();
                        
                        // Set the needed timout
                        void setHandler(chaos::common::data::CDataWrapper *data);
                        
                        void ccHandler();
                        
                        bool timeoutHandler();
                    };
                    
                }
            }
        }
    }
}

#endif /* SCWaitCommand_hpp */

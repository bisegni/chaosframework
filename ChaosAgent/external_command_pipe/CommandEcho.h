/*
 *	CommandEcho.h
 *
 *	!CHAOS [CHAOSFramework]
 *	Created by bisegni.
 *
 *    	Copyright 24/03/2017 INFN, National Institute of Nuclear Physics
 *
 *    	Licensed under the Apache License, Version 2.0 (the "License");
 *    	you may not use this file except in compliance with the License.
 *    	You may obtain a copy of the License at
 *
 *    	http://www.apache.org/licenses/LICENSE-2.0
 *
 *    	Unless required by applicable law or agreed to in writing, software
 *    	distributed under the License is distributed on an "AS IS" BASIS,
 *    	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    	See the License for the specific language governing permissions and
 *    	limitations under the License.
 */

#ifndef __CHAOSFramework__6596C3D_7BB5_4DF5_94F5_70AF5CBF589C_CommandEcho_h
#define __CHAOSFramework__6596C3D_7BB5_4DF5_94F5_70AF5CBF589C_CommandEcho_h

#include "AbstractExternalCommand.h"
namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            class CommandEcho:
            public AbstractExternalCommand {
            public:
                CommandEcho(AbstractCommandOutputStream& _cmd_ostream,
                            common::message::MDSMessageChannel& _mds_message_channel);
                ~CommandEcho();
                int execute(ChaosStringVector input_parameter);
            };
        }
    }
}

#endif /* __CHAOSFramework__6596C3D_7BB5_4DF5_94F5_70AF5CBF589C_CommandEcho_h */
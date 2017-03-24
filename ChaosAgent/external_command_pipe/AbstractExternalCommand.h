/*
 *	AbstractExternalCommand.h
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

#ifndef __CHAOSFramework_DE82735D_5860_4753_B2AD_FF830A202F96_AbstractExternalCommand_h
#define __CHAOSFramework_DE82735D_5860_4753_B2AD_FF830A202F96_AbstractExternalCommand_h

#include "AbstractCommandOutputStream.h"

#include <chaos/common/chaos_types.h>
#include <chaos/common/utility/NamedService.h>
#include <chaos/common/message/MDSMessageChannel.h>

#include <boost/shared_ptr.hpp>

namespace chaos {
    namespace agent {
        namespace external_command_pipe {
            
            //!base class for agent external command
            class AbstractExternalCommand:
            public common::utility::NamedService {
                AbstractCommandOutputStream& cmd_ostream;
            protected:
                common::message::MDSMessageChannel& mds_message_channel;
                void print(const std::string& line);
                void println(const std::string& line);
            public:
                AbstractExternalCommand(const std::string& service_name,
                                        AbstractCommandOutputStream& _cmd_ostream,
                                        common::message::MDSMessageChannel& _mds_message_channel);
                virtual ~AbstractExternalCommand();
                
                virtual int execute(ChaosStringVector input_parameter) = 0;
            };

            typedef boost::shared_ptr<AbstractExternalCommand> AbstractExternalCommandShrdPtr;
        }
    }
}

#endif /* __CHAOSFramework_DE82735D_5860_4753_B2AD_FF830A202F96_AbstractExternalCommand_h */

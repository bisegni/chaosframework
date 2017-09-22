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

            typedef ChaosSharedPtr<AbstractExternalCommand> AbstractExternalCommandShrdPtr;
        }
    }
}

#endif /* __CHAOSFramework_DE82735D_5860_4753_B2AD_FF830A202F96_AbstractExternalCommand_h */

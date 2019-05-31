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

#ifndef __CHAOSFramework__257C4FE_DCA8_4943_B997_F7B2F28195CC_chaos_agent_types_h
#define __CHAOSFramework__257C4FE_DCA8_4943_B997_F7B2F28195CC_chaos_agent_types_h

namespace chaos {
    namespace agent {
 
        typedef struct ChaosAgentSettings {
            std::string agent_uid;
            //!working directory for deploy and execution of chaos binary
            std::string working_directory;
            //!external command enable state
            bool ext_cmd_enabled;
            //!enable separate log file for us
            bool enable_us_logging;
            //!enable the us logging merged with agent
            bool enable_us_merge_logging;
            /*!
             External command pipe are two named pipe created for receive command(.in)
             in the .in one and send answer(.out)
             */
            //!the path of the ext command pipe
            std::string ext_cmd_pipe_path;
            //!the name of the ext command pipe
            std::string ext_cmd_pipe_name;
            
            // default script directory
            std::string script_dir;
            //!port where the rest service answer

            int restport;
        } ChaosAgentSettings;
        
    }
}

#endif /* __CHAOSFramework__257C4FE_DCA8_4943_B997_F7B2F28195CC_chaos_agent_types_h */

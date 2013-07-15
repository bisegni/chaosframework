/*
 *	ChannelSharedSetting.h
 *	!CHAOS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2013 INFN, National Institute of Nuclear Physics
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

#ifndef __CHAOSFramework__ChannelSharedSetting__
#define __CHAOSFramework__ChannelSharedSetting__

#include <map>
#include <string>

#include <boost/thread.hpp>

#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommandSandbox.h>

namespace chaos{
    namespace cu {
        namespace control_manager {
            namespace slow_command {
                
                using namespace std;
                
                class ChannelSharedSetting {
                    
                    boost::mutex mutextAccessMap;
                    
                    map<string, ChannelSetting*> channelSharedMap;
                };
                
            }
        }
    }
}



#endif /* defined(__CHAOSFramework__ChannelSharedSetting__) */

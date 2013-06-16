/*
 *	SlowCommand.cpp
 *	!CHOAS
 *	Created by Bisegni Claudio.
 *
 *    	Copyright 2012 INFN, National Institute of Nuclear Physics
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

#include <chaos/cu_toolkit/ControlManager/slcmd/SlowCommand.h>

using namespace chaos::cu::cm::slcmd;

/*
 Start the slow command sequence
 */
uint8_t SlowCommand::setHandler() {
    return 0;
}

/*
 implemente thee data acquisition for the command
 */
uint8_t SlowCommand::acquireHandler() {
    return 0;
}

/*
 Performe correlation and send command to the driver
 */
uint8_t SlowCommand::ccHandler() {
    return 0;
}